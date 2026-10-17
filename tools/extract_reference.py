import argparse
import collections
import csv
import hashlib
import io
import json
from pathlib import Path, PurePosixPath
import struct
import subprocess
import types


def digest(data):
    return hashlib.sha256(data).hexdigest()


def load_decoder(root, name):
    source = (root / (name + '.py')).read_text()
    source = source.split('if __name__ == "__main__":')[0]
    source = source.replace("'\\0' *", "b'\\0' *")
    source = source.replace(' / 2', ' // 2')
    source = source.replace('.tostring()', '.tobytes()')
    source = source.replace('f.read(4)', 'infile.read(4)')
    source = source.replace('outfile.write(chr(decode_table[next_offset]))',
                            'outfile.write(bytes([decode_table[next_offset]]))')
    module = types.ModuleType(name)
    exec(compile(source, str(root / (name + '.py')), 'exec'), module.__dict__)
    return module


def u8_entries(data):
    if data[:4] != b'U\xaa8-':
        raise ValueError('Not U8')
    root, header_size, data_start = struct.unpack_from('>III', data, 4)
    count = struct.unpack_from('>I', data, root + 8)[0]
    strings = root + count * 12
    if not (count > 0 and strings <= root + header_size <= data_start <= len(data)):
        raise ValueError('Invalid U8 header bounds')
    stack = [(count, PurePosixPath())]
    entries = []
    for index in range(1, count):
        while index >= stack[-1][0]:
            stack.pop()
        kind_name, offset, size = struct.unpack_from('>III', data, root + index * 12)
        kind, name_offset = kind_name >> 24, kind_name & 0xffffff
        start = strings + name_offset
        end = data.index(b'\0', start, root + header_size)
        name = data[start:end].decode('utf-8')
        normalized = PurePosixPath(name.replace('\\', '/'))
        if normalized.is_absolute() or '..' in normalized.parts or ':' in name:
            raise ValueError('Unsafe U8 entry name')
        path = stack[-1][1] / normalized
        if kind == 1:
            if not index < size <= stack[-1][0]:
                raise ValueError('Invalid U8 directory bounds')
            stack.append((size, path))
        elif kind == 0:
            if not data_start <= offset <= offset + size <= len(data):
                raise ValueError('Invalid U8 file bounds')
            entries.append((path.as_posix(), offset, size, name))
        else:
            raise ValueError('Invalid U8 node type')
    names = [name.casefold() for name, _, _, _ in entries]
    if len(set(names)) != len(names):
        raise ValueError('Duplicate U8 paths')
    return entries


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dtk', type=Path, default=Path('dtk.exe'))
    parser.add_argument('--decoders', type=Path, default=Path('tools/local/vcromclaim'))
    parser.add_argument('--wad', type=Path, default=Path('orig/wpse-v1/reference.wad'))
    parser.add_argument('--output', type=Path, default=Path('orig/wpse-v1/extraction'))
    parser.add_argument('--report', type=Path, default=Path('docs/re/wpse-v1-extraction.json'))
    args = parser.parse_args()
    if args.output.exists():
        raise ValueError('Output already exists; choose a fresh output directory')
    wad_hash = digest(args.wad.read_bytes())
    if wad_hash != '3edf8518c33ea03a04a0c519ae586a29e9b40a32327be72623277773ac88f680':
        raise ValueError('WAD differs from frozen WPSE reference')
    if digest(args.dtk.read_bytes()) != 'a89f8a1ab07cde5af83218da4247dfd535ad17f2fdb69bf361a966eec7b8fe25':
        raise ValueError('Unexpected dtk binary')
    revision = subprocess.check_output(['git', '-C', str(args.decoders), 'rev-parse', 'HEAD'], text=True).strip()
    if revision != '6e537af6d314c3d8a6c7353645c137572b37b4b6':
        raise ValueError('Unexpected supporting decoder revision')
    expected_sources = {
        'lzh8': 'ec324292f1b167027b0c4fc70757ab58435ebf3af22051bd17aebaff5a14ff3a',
        'huf8': '512d4207e0cfb73816728e7d22c8d089af8e342111ece04c254b37a770db2c30',
    }
    for name, expected in expected_sources.items():
        if digest((args.decoders / (name + '.py')).read_bytes()) != expected:
            raise ValueError('Supporting decoder source differs from pinned bytes')
    decoders = {name: load_decoder(args.decoders, name) for name in ('lzh8', 'huf8')}
    args.output.mkdir(parents=True)
    commands = []

    def run(*parts):
        command = [str(args.dtk.resolve()), *map(str, parts)]
        result = subprocess.run(command, capture_output=True, text=True)
        commands.append(dict(arguments=list(map(str, parts)), exit_code=result.returncode,
                             stdout=result.stdout, stderr=result.stderr))
        if result.returncode:
            raise RuntimeError(result.stderr)

    raw = args.output / 'contents'
    run('wad', 'extract', args.wad, '-o', raw)
    tmd = next(raw.glob('*.tmd')).read_bytes()
    count = struct.unpack_from('>H', tmd, 0x1de)[0]
    for index in range(count):
        content, _, _, size, expected = struct.unpack_from('>IHHQ20s', tmd, 0x1e4 + index * 36)
        data = (raw / f'{content:08x}.app').read_bytes()
        if len(data) != size or hashlib.sha1(data).digest() != expected:
            raise ValueError('Extracted content differs from TMD')
    queue = collections.deque((p, None, p.name, 0) for p in sorted(raw.iterdir()) if p.is_file())
    records = []
    stats = collections.Counter()

    def emit(parent, data, operation, filename, logical, depth):
        dest = args.output / 'expanded' / f'{parent:05d}' / filename
        dest.parent.mkdir(parents=True, exist_ok=True)
        with dest.open('xb') as stream:
            stream.write(data)
        queue.append((dest, parent, logical + '::' + operation, depth + 1))

    while queue:
        path, parent, logical, depth = queue.popleft()
        if depth > 20:
            raise ValueError('Unexpected archive depth')
        data = path.read_bytes()
        ident = len(records)
        record = dict(id=ident, path=path.as_posix(), parent=parent, logical_path=logical,
                      size=len(data), sha256=digest(data), magic=data[:16].hex(), action='retained')
        records.append(record)
        if data[:4] == b'U\xaa8-':
            entries = u8_entries(data)
            dest = args.output / 'expanded' / f'{ident:05d}' / 'files'
            for name, _, _, _ in entries:
                (dest / name).parent.mkdir(parents=True, exist_ok=True)
            run('u8', 'extract', path, '-o', dest, '--no-decompress')
            for name, offset, size, _ in entries:
                child = dest / name
                if child.read_bytes() != data[offset:offset + size]:
                    raise ValueError('U8 extraction differs from archive bytes')
                queue.append((child, ident, logical + '::' + name, depth + 1))
            if sum(p.is_file() for p in dest.rglob('*')) != len(entries):
                raise ValueError('U8 output count mismatch')
            record.update(action='u8', entries=len(entries), archive_entries=[dict(path=n, offset=o, size=s, original_name=r) for n, o, s, r in entries])
        elif path.name in ('00000000.app', '0001000157505345.trailer') and data[0x640:0x644] == b'U\xaa8-':
            u8_entries(data[0x640:])
            emit(ident, data[0x640:], 'banner-u8@0x640', 'banner.arc', logical, depth)
            record.update(action='banner-wrapper', offset=0x640)
        elif data[:4] == b'IMD5':
            size = struct.unpack_from('>I', data, 4)[0]
            payload = data[32:32 + size]
            if len(payload) != size or hashlib.md5(payload).digest() != data[16:32]:
                raise ValueError('IMD5 payload mismatch')
            emit(ident, payload, 'imd5-payload', 'payload.bin', logical, depth)
            record['action'] = 'imd5'
        elif data[:4] == b'LZ77':
            emit(ident, data[4:], 'lz77-wrapper', 'payload.bin', logical, depth)
            record['action'] = 'lz77-wrapper'
        elif data and data[0] in (0x10, 0x11, 0x28, 0x40) and (
            path.suffix in ('.cx', '.arc', '.brsar') or path.name == 'payload.bin' or path.name == '00000001.app'
        ):
            expected = int.from_bytes(data[1:4], 'little')
            if expected == 0:
                expected = int.from_bytes(data[4:8], 'little')
            if not 0 < expected < 256 * 1024 * 1024:
                raise ValueError('Unexpected decoded size')
            name = 'main.dol' if path.name == '00000001.app' else 'decoded' + ('.arc' if '.arc' in path.name else '.bin')
            if data[0] in (0x10, 0x11):
                dest = args.output / 'expanded' / f'{ident:05d}' / name
                dest.parent.mkdir(parents=True, exist_ok=True)
                run('nlzss', 'decompress', path, '-o', dest)
                output = dest.read_bytes()
                queue.append((dest, ident, logical + '::nlzss', depth + 1))
                record['action'] = 'nlzss'
            else:
                if data[0] == 0x40:
                    output = decoders['lzh8'].decompress(io.BytesIO(data))
                    operation = 'lzh8'
                else:
                    stream = io.BytesIO()
                    decoders['huf8'].decompress(io.BytesIO(data), stream)
                    output = stream.getvalue()
                    operation = 'huf8'
                emit(ident, output, operation, name, logical, depth)
                record['action'] = operation
            if len(output) != expected:
                raise ValueError('Decompressed length mismatch')
        stats[record['action']] += 1
        if ident % 250 == 0:
            print(f'Inspected {ident + 1} files; queued {len(queue)}', flush=True)
    if digest(args.wad.read_bytes()) != wad_hash:
        raise ValueError('Reference changed')
    report = dict(wad_sha256=wad_hash, dtk_sha256=digest(args.dtk.read_bytes()),
                  decoder_revision=revision, decoder_sha256={name: digest((args.decoders / (name + '.py')).read_bytes()) for name in decoders},
                  summary=dict(stats), records=records, commands=commands)
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + '\n')
    with args.report.with_suffix('.csv').open('w', newline='', encoding='utf-8') as stream:
        fields = ['id', 'parent', 'path', 'logical_path', 'size', 'sha256', 'magic', 'action']
        writer = csv.DictWriter(stream, fieldnames=fields, extrasaction='ignore')
        writer.writeheader()
        writer.writerows(records)
    print(json.dumps(dict(stats)))


if __name__ == '__main__':
    main()
