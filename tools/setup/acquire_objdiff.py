import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import subprocess
import sys
import tempfile
import urllib.request
import zipfile


ROOT = Path(__file__).resolve().parents[2]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def verify(path, expected, size=None):
    if not path.is_file() or (size is not None and path.stat().st_size != size) or digest(path) != expected:
        raise ValueError(f'Wrong or missing pinned file: {path}')


def child(directory, relative):
    name = PurePosixPath(relative)
    if name.is_absolute() or not name.parts or any(part in ('.', '..') or ':' in part or '\\' in part for part in name.parts):
        raise ValueError(f'Unsafe archive or patch path: {relative}')
    result = directory.joinpath(*name.parts).resolve()
    if not result.is_relative_to(directory.resolve()):
        raise ValueError(f'Path escapes directory: {relative}')
    return result


def apply_patch(source, recipe):
    for entry in recipe['files']:
        path = child(source, entry['path'])
        verify(path, entry['original_sha256'])
        lines = path.read_text(encoding='utf-8').splitlines(keepends=True)
        previous = len(lines) + 1
        for edit in reversed(entry['edits']):
            start = edit['start_line']
            before = edit['before'].splitlines(keepends=True)
            end = start + len(before)
            if start < 0 or end > previous or lines[start:end] != before:
                raise ValueError(f'Patch context differs at {path}:{start + 1}')
            lines[start:end] = edit['after'].splitlines(keepends=True)
            previous = start
        patched = ''.join(lines).encode('utf-8')
        if hashlib.sha256(patched).hexdigest() != entry['patched_sha256']:
            raise ValueError(f'Patched digest differs at {path}')
        path.write_bytes(patched)


def acquire_archive(source, local, offline):
    archive = local / 'upstream.zip'
    if not archive.exists():
        if offline:
            raise ValueError(f'Offline source archive missing: {archive}')
        partial = local / 'upstream.zip.part'
        with urllib.request.urlopen(source['url'], timeout=120) as response, partial.open('wb') as output:
            shutil.copyfileobj(response, output)
        verify(partial, source['sha256'], source['size'])
        partial.replace(archive)
    verify(archive, source['sha256'], source['size'])
    return archive


def build(manifest, recipe, local, offline):
    archive = acquire_archive(manifest['source'], local, offline)
    versions = {}
    for tool in ('rustc', 'cargo'):
        result = subprocess.run([tool, '--version'], capture_output=True, text=True, check=True)
        versions[tool] = result.stdout.strip()
        if versions[tool] != manifest['host'][tool]:
            raise ValueError(f"{tool} version differs: {versions[tool]}; expected {manifest['host'][tool]}")
    sysroot = Path(subprocess.run(['rustc', '--print', 'sysroot'], capture_output=True, text=True, check=True).stdout.strip())
    for tool in ('rustc', 'cargo'):
        verify(sysroot / 'bin' / (tool + '.exe'), manifest['host'][tool + '_sha256'])
    directory = Path(tempfile.mkdtemp(prefix='build-', dir=local))
    with zipfile.ZipFile(archive) as package:
        for member in package.infolist():
            path = child(directory, member.filename)
            if member.is_dir():
                path.mkdir(parents=True, exist_ok=True)
            else:
                path.parent.mkdir(parents=True, exist_ok=True)
                with package.open(member) as stream, path.open('wb') as output:
                    shutil.copyfileobj(stream, output)
    source = child(directory, manifest['source']['directory'])
    verify(source / 'Cargo.lock', manifest['cargo_lock_sha256'])
    apply_patch(source, recipe)
    environment = os.environ.copy()
    cargo_home = Path(environment.get('CARGO_HOME', Path.home() / '.cargo')).resolve()
    for key in ('RUSTFLAGS', 'CARGO_ENCODED_RUSTFLAGS', 'CARGO_BUILD_RUSTFLAGS'):
        environment.pop(key, None)
    environment['CARGO_ENCODED_RUSTFLAGS'] = '\x1f'.join([
        f'--remap-path-prefix={source}=/objdiff',
        f'--remap-path-prefix={cargo_home}=/cargo',
        f'--remap-path-prefix={directory / "target"}=/target',
        '-C',
        'link-arg=/Brepro',
    ])
    environment['CARGO_TARGET_DIR'] = str(directory / 'target')
    command = ['cargo', 'build', '--release', '--locked', '-p', 'objdiff-cli']
    if offline:
        command.append('--offline')
    subprocess.run(command, cwd=source, env=environment, check=True)
    executable = directory / 'target/release/objdiff-cli.exe'
    verify(executable, manifest['binary']['sha256'], manifest['binary']['size'])
    return executable, versions


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--manifest', type=Path, default=ROOT / 'config/objdiff.json')
    parser.add_argument('--local', type=Path, default=ROOT / 'tools/local/objdiff')
    parser.add_argument('--output', type=Path, default=ROOT / 'tools/local/objdiff-cli.exe')
    parser.add_argument('--offline', action='store_true')
    parser.add_argument('--rebuild', action='store_true')
    args = parser.parse_args()
    manifest_digest = digest(args.manifest)
    manifest = json.loads(args.manifest.read_text(encoding='utf-8'))
    if manifest['schema_version'] != 1:
        raise ValueError('Unsupported objdiff build manifest')
    storage = (ROOT / 'tools/local').resolve()
    local = args.local.resolve()
    output = args.output.resolve()
    if not local.is_relative_to(storage) or not output.is_relative_to(storage):
        raise ValueError('Tool storage and executable output must remain inside tools/local')
    patch = child(ROOT, manifest['patch']['path'])
    verify(patch, manifest['patch']['sha256'])
    recipe = json.loads(patch.read_text(encoding='utf-8'))
    if recipe['schema_version'] != 1:
        raise ValueError('Unsupported objdiff patch recipe')
    if output.is_file() and not args.rebuild and output.stat().st_size == manifest['binary']['size'] and digest(output) == manifest['binary']['sha256']:
        print(f'objdiff-cli: cached, digest verified ({manifest["version"]})')
        return 0
    if args.offline and not args.rebuild:
        raise ValueError(f'Offline executable missing: {output}')
    local.mkdir(parents=True, exist_ok=True)
    executable, versions = build(manifest, recipe, local, args.offline)
    verify(args.manifest, manifest_digest)
    verify(patch, manifest['patch']['sha256'])
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(prefix=output.name + '.', suffix='.part', dir=output.parent, delete=False) as stream:
        temporary = Path(stream.name)
    try:
        shutil.copyfile(executable, temporary)
        verify(temporary, manifest['binary']['sha256'], manifest['binary']['size'])
        temporary.replace(output)
    finally:
        temporary.unlink(missing_ok=True)
    receipt = {
        'schema_version': 1,
        'manifest_sha256': manifest_digest,
        'patch_sha256': digest(patch),
        'source_sha256': manifest['source']['sha256'],
        'cargo_lock_sha256': manifest['cargo_lock_sha256'],
        'binary_sha256': digest(output),
        'host': versions,
    }
    (local / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n', encoding='utf-8')
    print(f'objdiff-cli: rebuilt and digest verified ({manifest["version"]})')
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, subprocess.CalledProcessError, zipfile.BadZipFile) as error:
        print(f'error: {error}', file=sys.stderr)
        sys.exit(1)
