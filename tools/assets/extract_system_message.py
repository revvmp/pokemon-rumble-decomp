import argparse
import hashlib
import json
from pathlib import Path
import struct


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('reference', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--c-include', type=Path, required=True)
    args = parser.parse_args()
    try:
        manifest = Path(__file__).resolve().parents[2] / 'config/targets/wpse-v1.json'
        expected = json.loads(manifest.read_text(encoding='utf-8'))['artifacts'][0]
        data = args.reference.read_bytes()
        if len(data) != expected['size'] or hashlib.sha256(data).hexdigest() != expected['sha256']:
            raise ValueError('Supply the original WPSE USA v1 main.dol; its hash must match config/targets/wpse-v1.json.')
        address = 0x80363A80
        size = 11488
        offsets = struct.unpack_from('>18I', data, 0)
        addresses = struct.unpack_from('>18I', data, 0x48)
        sizes = struct.unpack_from('>18I', data, 0x90)
        archive = None
        for offset, start, length in zip(offsets, addresses, sizes):
            if start <= address and address + size <= start + length:
                position = offset + address - start
                archive = data[position:position + size]
                break
        if archive is None or hashlib.sha256(archive).hexdigest() != 'e27d380bbad3aff2ef2d41cfdc5b77590390a68937fef6edc87502f2d1813161':
            raise ValueError('Embedded system message archive does not match the expected asset.')
        if len({args.reference.resolve(), args.output.resolve(), args.c_include.resolve()}) != 3:
            raise ValueError('Input and output paths must be distinct.')
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(archive)
        args.c_include.parent.mkdir(parents=True, exist_ok=True)
        lines = [', '.join(f'0x{value:02X}' for value in archive[offset:offset + 16]) + ',' for offset in range(0, len(archive), 16)]
        args.c_include.write_text('\n'.join(lines) + '\n', encoding='ascii', newline='\n')
    except (OSError, ValueError, struct.error) as error:
        parser.exit(1, f'{error}\n')


if __name__ == '__main__':
    main()
