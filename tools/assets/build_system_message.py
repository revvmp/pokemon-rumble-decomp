import argparse
import hashlib
import json
import struct
import zlib
from pathlib import Path

from u8_archive import encode_archive
from nw4r_layout import encode_animation, encode_layout
from gx_texture import decode_png, encode_texture
from system_message_schema import validate_animation, validate_layout, validate_manifest, validate_texture


def unique_fields(pairs):
    result = {}
    for name, value in pairs:
        if name in result:
            raise ValueError(f'Duplicate JSON field: {name}')
        result[name] = value
    return result


def reject_constant(value):
    raise ValueError(f'Invalid JSON number: {value}')


def read_json(path):
    return json.loads(path.read_text(encoding='utf-8'), object_pairs_hook=unique_fields, parse_constant=reject_constant)


def build_assets(manifest_path, output_path, member_output=None, include_output=None):
    manifest = read_json(manifest_path)
    inputs = manifest_path.parent
    validate_manifest(manifest)
    input_paths = {manifest_path.resolve()}
    members = {}
    reports = []
    for asset in manifest['assets']:
        source = (inputs / asset['source']).resolve()
        if not source.is_relative_to(inputs.resolve()):
            raise ValueError('Asset source is outside the input directory')
        input_paths.add(source)
        metadata = read_json(source)
        if asset['type'] == 'layout':
            validate_layout(metadata)
            encoded = encode_layout(metadata)
        elif asset['type'] == 'animation':
            validate_animation(metadata)
            encoded = encode_animation(metadata)
        elif asset['type'] == 'texture':
            validate_texture(metadata)
            image = (source.parent / metadata['image']).resolve()
            if not image.is_relative_to(inputs.resolve()):
                raise ValueError('Texture image is outside the input directory')
            input_paths.add(image)
            width, height, pixels = decode_png(image.read_bytes())
            encoded = encode_texture(metadata, width, height, pixels)
        else:
            raise ValueError(f'Unsupported asset type: {asset["type"]}')
        if asset['path'] in members:
            raise ValueError('Duplicate asset path')
        members[asset['path']] = encoded
        reports.append({'path': asset['path'], 'size': len(encoded), 'sha256': hashlib.sha256(encoded).hexdigest()})
    archive = encode_archive(members)
    destinations = []
    if member_output is not None:
        for path, data in members.items():
            destination = (member_output / path).resolve()
            if not destination.is_relative_to(member_output.resolve()):
                raise ValueError('Archive member output is outside the output directory')
            destinations.append((destination, data))
    output_paths = [output_path.resolve()] + [destination for destination, _ in destinations]
    if include_output is not None:
        output_paths.append(include_output.resolve())
    if len(output_paths) != len(set(output_paths)):
        raise ValueError('Output paths overlap')
    if any(path in input_paths for path in output_paths):
        raise ValueError('An output path overlaps an asset input')
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_bytes(archive)
    for destination, data in destinations:
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_bytes(data)
    if include_output is not None:
        include_output.parent.mkdir(parents=True, exist_ok=True)
        lines = [', '.join(f'0x{value:02X}' for value in archive[offset:offset + 16]) + ',' for offset in range(0, len(archive), 16)]
        include_output.write_text('\n'.join(lines) + '\n', encoding='ascii', newline='\n')
    return {'size': len(archive), 'sha256': hashlib.sha256(archive).hexdigest(), 'members': reports}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('manifest', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--members', type=Path)
    parser.add_argument('--c-include', type=Path)
    args = parser.parse_args()
    try:
        result = build_assets(args.manifest, args.output, args.members, args.c_include)
    except (OSError, ValueError, struct.error, zlib.error) as error:
        parser.error(str(error))
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
