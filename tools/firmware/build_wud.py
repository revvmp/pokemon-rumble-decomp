import argparse
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path


sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from tools.dsp.build_dsp import BuildError, depfile_path, file_path, output_path, publish, validate_collisions, write_temporary


def unique_fields(pairs):
    result = {}
    for name, value in pairs:
        if name in result:
            raise BuildError(f'Duplicate JSON field: {name}')
        result[name] = value
    return result


def capture(path, snapshots):
    data = path.read_bytes()
    if path in snapshots and snapshots[path] != data:
        raise BuildError(f'Controller input changed during validation: {path}')
    snapshots[path] = data
    return data


def read_json(path, snapshots):
    return json.loads(capture(path, snapshots).decode('utf-8'), object_pairs_hook=unique_fields)


def local_input(root, name, suffix):
    if not isinstance(name, str) or re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*' + re.escape(suffix), name) is None:
        raise BuildError(f'Expected a local {suffix} source name: {name!r}')
    path = file_path(root / name)
    if path.parent != root:
        raise BuildError(f'Source resolves outside its manifest directory: {name}')
    return path


def read_sources(manifest_path, snapshots):
    manifest = read_json(manifest_path, snapshots)
    fields = {'schema_version', 'firmware', 'linker_script', 'payload', 'patches'}
    if not isinstance(manifest, dict) or set(manifest) != fields or type(manifest['schema_version']) is not int or manifest['schema_version'] != 1:
        raise BuildError('Unsupported controller manifest schema')
    if manifest['firmware'] != 'BCM2045B3_002.003.014.0167':
        raise BuildError('Unsupported controller firmware')
    if not isinstance(manifest['patches'], list) or len(manifest['patches']) != 7:
        raise BuildError('Expected exactly seven patch records')
    records = [manifest['payload'], *manifest['patches']]
    sources = []
    ranges = []
    slots = []
    for index, record in enumerate(records):
        expected = {'source', 'destination'} if index == 0 else {'slot', 'source', 'destination'}
        if not isinstance(record, dict) or set(record) != expected:
            raise BuildError('Unexpected controller record fields')
        address = record['destination']
        size = 180 if index == 0 else 8
        if type(address) is not int or address < 0 or address + size > 0x100000000 or address & 1:
            raise BuildError('Controller destinations must be bounded even Thumb addresses')
        if any(address < end and begin < address + size for begin, end in ranges):
            raise BuildError('Controller code ranges overlap')
        ranges.append((address, address + size))
        if index:
            slot = record['slot']
            if type(slot) is not int or not 0 <= slot <= 255 or slot in slots:
                raise BuildError('Patch slots must be distinct bytes')
            slots.append(slot)
        source = local_input(manifest_path.parent, record['source'], '.s')
        text = capture(source, snapshots).decode('utf-8')
        if re.search(r'\.(?:include|incbin)\b', text, re.I):
            raise BuildError(f'Controller assembly must be self-contained: {source}')
        sources.append(source)
    if len(set(sources)) != len(sources):
        raise BuildError('Controller source names must be distinct')
    linker = local_input(manifest_path.parent, manifest['linker_script'], '.ld')
    if re.search(r'\b(?:INCLUDE|INPUT|GROUP|SEARCH_DIR|STARTUP)\b', capture(linker, snapshots).decode('utf-8'), re.I):
        raise BuildError('Controller linker script must not import additional files')
    return records, sources, linker


def compiler_path(tool_install, tool_manifest, snapshots):
    install = Path(tool_install).expanduser().resolve()
    receipt_path = file_path(install / 'build.json')
    receipt = read_json(receipt_path, snapshots)
    manifest = read_json(tool_manifest, snapshots)
    if receipt.get('manifest_sha256') != hashlib.sha256(snapshots[tool_manifest]).hexdigest() or receipt.get('compiler_exit') != 0:
        raise BuildError('Pinned tool receipt does not match its manifest; run acquire_dsp_assembler.py')
    compiler = manifest['compiler']
    directory = (install / receipt['build_directory']).resolve()
    if not directory.is_relative_to(install):
        raise BuildError('Pinned compiler build directory escapes the tool install')
    path = file_path(directory / 'compiler' / compiler['root'] / compiler['executable'])
    if not path.is_relative_to(directory) or hashlib.sha256(capture(path, snapshots)).hexdigest() != compiler['executable_sha256']:
        raise BuildError('Pinned Zig compiler digest differs')
    return path, receipt_path


def elf_sections(path):
    data = path.read_bytes()
    if data[:6] != b'\x7fELF\x01\x01':
        raise BuildError('Controller link did not produce a little-endian ELF32 file')
    header = struct.unpack_from('<16sHHIIIIIHHHHHH', data)
    if header[2] != 40 or header[11] != 40:
        raise BuildError('Controller link did not produce an ARM ELF')
    raw = [struct.unpack_from('<10I', data, header[6] + index * header[11]) for index in range(header[12])]
    strings = raw[header[13]]
    names = data[strings[4]:strings[4] + strings[5]]
    result = {}
    for entry in raw:
        name = names[entry[0]:names.find(b'\0', entry[0])].decode('ascii')
        if entry[2] & 2 and entry[5]:
            result[name] = {'address': entry[3], 'size': entry[5], 'flags': entry[2]}
    return result


def run(command, directory, environment):
    result = subprocess.run([str(value) for value in command], cwd=directory, env=environment, capture_output=True)
    if result.returncode:
        diagnostic = (result.stdout + result.stderr).decode('utf-8', 'replace')
        raise BuildError(f'Controller tool exited {result.returncode}:\n{diagnostic}')


def byte_initializer(data):
    return ''.join('    ' + ', '.join(f'0x{value:02X}' for value in data[offset:offset + 16]) + ',\n' for offset in range(0, len(data), 16))


def build(args):
    manifest_path = file_path(args.manifest)
    tool_manifest = file_path(args.tool_manifest)
    snapshots = {}
    records, sources, linker = read_sources(manifest_path, snapshots)
    compiler, receipt = compiler_path(args.tool_install, tool_manifest, snapshots)
    helper = Path(__file__).resolve()
    publisher = helper.parents[1] / 'dsp/build_dsp.py'
    inputs = [manifest_path, tool_manifest, receipt, compiler, helper, publisher, linker, *sources]
    for path in inputs:
        capture(path, snapshots)
    requested = [args.binary, args.commands_binary, args.c_include, args.commands_include, args.depfile]
    outputs = [output_path(path) for path in requested]
    validate_collisions(inputs, outputs)
    targets = ' '.join(depfile_path(Path(path).expanduser()) for path in requested[:-1])
    prerequisites = ' '.join(depfile_path(path) for path in sorted(set(inputs)))
    dependency_text = (targets + ': ' + prerequisites + '\n').encode('utf-8')
    outputs[0].parent.mkdir(parents=True, exist_ok=True)
    staged = {}
    try:
        with tempfile.TemporaryDirectory(prefix='.wud-', dir=outputs[0].parent) as temporary:
            directory = Path(temporary).resolve()
            if not directory.is_relative_to(outputs[0].parent):
                raise BuildError('Controller temporary directory escaped the output directory')
            environment = os.environ.copy()
            environment['ZIG_GLOBAL_CACHE_DIR'] = str(directory / 'global-cache')
            environment['ZIG_LOCAL_CACHE_DIR'] = str(directory / 'local-cache')
            objects = []
            for source in sources:
                output = directory / (source.stem + '.o')
                run([compiler, 'cc', '-target', 'thumb-freestanding-eabi', '-mcpu=arm7tdmi', '-c', source, '-o', output], directory, environment)
                objects.append(output)
            elf = directory / 'wud_patch.elf'
            run([compiler, 'ld.lld', '-m', 'armelf', '-T', linker, '-o', elf, *objects], directory, environment)
            sections = elf_sections(elf)
            expected_sections = {'.text.' + source.stem for source in sources}
            if set(sections) != expected_sections:
                raise BuildError('Unexpected allocated controller ELF sections')
            blocks = []
            for index, (record, source) in enumerate(zip(records, sources)):
                name = '.text.' + source.stem
                expected_size = 180 if index == 0 else 8
                section = sections[name]
                if section['address'] != record['destination'] or section['size'] != expected_size or section['flags'] & 4 == 0:
                    raise BuildError(f'Controller section has wrong address, size or flags: {name}')
                binary = directory / (source.stem + '.bin')
                run([compiler, 'objcopy', '-O', 'binary', '--only-section=' + name, elf, binary], directory, environment)
                data = binary.read_bytes()
                if len(data) != expected_size:
                    raise BuildError(f'Controller block has wrong size: {source}')
                blocks.append(data)
            patch = struct.pack('<II', records[0]['destination'], len(blocks[0])) + blocks[0]
            commands = bytes([len(records) - 1]) + b''.join(struct.pack('<BI', record['slot'], record['destination']) + data for record, data in zip(records[1:], blocks[1:]))
            if len(patch) != 188 or len(commands) != 92:
                raise BuildError('Controller protocol objects have wrong sizes')
            rows = []
            for record, data in zip(records[1:], blocks[1:]):
                address = ', '.join(f'0x{value:02X}' for value in struct.pack('<I', record['destination']))
                instructions = ', '.join(f'0x{value:02X}' for value in data)
                rows.append(f'        {{0x{record["slot"]:02X}, {{{address}}}, {{{instructions}}}}},')
            command_initializer = f'    {len(rows)},\n    {{\n' + '\n'.join(rows) + '\n    }\n'
            contents = [patch, commands, byte_initializer(patch).encode('ascii'), command_initializer.encode('ascii'), dependency_text]
            for path, snapshot in snapshots.items():
                if path.read_bytes() != snapshot:
                    raise BuildError(f'Controller build input changed: {path}')
            for output, content in zip(outputs, contents):
                staged[output] = write_temporary(output, content)
            validate_collisions(inputs, outputs)
            publish(staged)
    finally:
        for path in staged.values():
            path.unlink(missing_ok=True)
    print(f'Built {len(patch)} controller payload bytes and {len(commands)} installation bytes from {len(sources)} assembly sources')


def main():
    parser = argparse.ArgumentParser(description='Build the BCM2045 controller patch and typed C initializers')
    for option in ('manifest', 'tool-install', 'tool-manifest', 'binary', 'commands-binary', 'c-include', 'commands-include', 'depfile'):
        parser.add_argument('--' + option, required=True)
    args = parser.parse_args()
    try:
        build(args)
    except (BuildError, OSError, ValueError, KeyError, TypeError, struct.error) as error:
        print(f'Controller build failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
