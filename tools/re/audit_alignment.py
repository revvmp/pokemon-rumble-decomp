import argparse
import csv
import json
import struct
from pathlib import Path


def dol_ranges(path):
    data = path.read_bytes()
    if len(data) < 0x100:
        raise ValueError(f"Truncated DOL: {path}")
    words = struct.unpack_from(">57I", data)
    ranges = []
    for index in range(18):
        offset, address, size = words[index], words[18 + index], words[36 + index]
        if not size:
            continue
        if offset < 0x100 or offset + size > len(data):
            raise ValueError(f"Invalid DOL section: {path}, {index}")
        ranges.append((address, address + size, offset))
    return data, ranges


def range_bytes(data, ranges, start, end):
    matches = [(address, offset) for address, limit, offset in ranges if address <= start < end <= limit]
    if len(matches) != 1:
        raise ValueError(f"Range outside one DOL section: {start:#x}-{end:#x}")
    address, offset = matches[0]
    return data[offset + start - address:offset + end - address]


def main():
    parser = argparse.ArgumentParser(description="Check documented native alignment ranges against original and rebuilt DOL bytes.")
    parser.add_argument("--ledger", type=Path, default=Path("docs/re/native-alignment.tsv"))
    parser.add_argument("--reference", type=Path, default=Path("orig/wpse-v1/main.dol"))
    parser.add_argument("--rebuilt", type=Path, default=Path("build/wpse-v1/main.dol"))
    args = parser.parse_args()
    reference, reference_ranges = dol_ranges(args.reference)
    rebuilt, rebuilt_ranges = dol_ranges(args.rebuilt)
    seen = set()
    count = 0
    total = 0
    with args.ledger.open(encoding="utf-8", newline="") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            start, end = int(row["start"], 0), int(row["end"], 0)
            alignment = int(row["alignment"])
            if alignment <= 1 or alignment & (alignment - 1):
                raise ValueError(f"Invalid alignment: {alignment}")
            if (start + alignment - 1) // alignment * alignment != end:
                raise ValueError(f"Range is not the documented alignment gap: {start:#x}-{end:#x}")
            if any(address in seen for address in range(start, end)):
                raise ValueError(f"Overlapping alignment range: {start:#x}-{end:#x}")
            seen.update(range(start, end))
            original = range_bytes(reference, reference_ranges, start, end)
            output = range_bytes(rebuilt, rebuilt_ranges, start, end)
            if any(original) or output != original:
                raise ValueError(f"Alignment bytes differ or contain nonzero data: {start:#x}-{end:#x}")
            count += 1
            total += end - start
    if not count:
        raise ValueError("Empty alignment ledger")
    print(json.dumps({"ranges": count, "bytes": total, "original_zero": True, "rebuilt_identical": True}))


if __name__ == "__main__":
    main()
