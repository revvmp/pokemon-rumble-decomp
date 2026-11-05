import argparse
import hashlib
import json
import sys
from pathlib import Path


DATA_SECTIONS = frozenset({".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2", ".ctors", ".dtors", "extab", "extabindex"})


def count(values, key):
    value = values.get(key, 0)
    if isinstance(value, str) and value.isdecimal():
        value = int(value)
    if type(value) is not int or value < 0:
        raise ValueError(f"Invalid count {key}: {value!r}")
    return value


def summarize(report, path, digest):
    if not isinstance(report, dict) or report.get("version") != 2:
        raise ValueError("Expected an objdiff version 2 report")
    measures = report["measures"]
    totals = {key: count(measures, key) for key in ("total_data", "matched_data", "complete_data")}
    totals["unmatched_data"] = totals["total_data"] - totals["matched_data"]
    if not 0 <= totals["matched_data"] <= totals["total_data"] or totals["complete_data"] > totals["total_data"]:
        raise ValueError("Data credit exceeds the report denominator")
    section_totals = {}
    unit_totals = {key: 0 for key in ("total_data", "matched_data", "complete_data")}
    units = []
    for unit in report["units"]:
        values = {key: count(unit["measures"], key) for key in unit_totals}
        for key in unit_totals:
            unit_totals[key] += values[key]
        lost = values["total_data"] - values["matched_data"]
        if not 0 <= values["matched_data"] <= values["total_data"] or values["complete_data"] > values["total_data"]:
            raise ValueError(f"Invalid data credit in {unit['name']}")
        if not lost:
            continue
        sections = []
        for section in unit.get("sections", []):
            name = section["name"]
            if name.split("$", 1)[0] not in DATA_SECTIONS:
                if name != ".init" and not name.startswith(".text"):
                    raise ValueError(f"Unclassified section {name!r} in {unit['name']}")
                continue
            score = section.get("fuzzy_match_percent")
            if score is not None and (not isinstance(score, (int, float)) or not 0 <= score <= 100):
                raise ValueError(f"Invalid section score in {unit['name']}: {score!r}")
            size = count(section, "size")
            if score == 100 or not size:
                continue
            sections.append({"name": name, "size": size, "fuzzy_match_percent": score})
            aggregate = section_totals.setdefault(name, {"name": name, "section_count": 0, "lost_credit": 0})
            aggregate["section_count"] += 1
            aggregate["lost_credit"] += size
        explained = sum(section["size"] for section in sections)
        if explained != lost:
            raise ValueError(f"Section credit does not reconcile for {unit['name']}: {explained} != {lost}")
        units.append({"name": unit["name"], **values, "lost_credit": lost, "sections": sections})
    for key, value in unit_totals.items():
        if value != totals[key]:
            raise ValueError(f"Unit sum differs from report {key}: {value} != {totals[key]}")
    return {
        "report": str(path),
        "sha256": digest,
        **totals,
        "reported_matched_data_percent": measures.get("matched_data_percent"),
        "units_with_data_gap": len(units),
        "section_credit_reconciled": True,
        "sections": sorted(section_totals.values(), key=lambda item: (-item["lost_credit"], item["name"])),
        "units": sorted(units, key=lambda item: (-item["lost_credit"], item["name"])),
    }


def main():
    parser = argparse.ArgumentParser(description="Audit actual objdiff data credit without modifying its report.")
    parser.add_argument("report", type=Path)
    parser.add_argument("--limit", type=int, default=25, help="Maximum ranked units in text output; 0 shows all")
    parser.add_argument("--json", action="store_true", help="Print the complete inventory as JSON")
    args = parser.parse_args()
    if args.limit < 0:
        parser.error("--limit must be nonnegative")
    try:
        raw = args.report.read_bytes()
        inventory = summarize(json.loads(raw), args.report, hashlib.sha256(raw).hexdigest())
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    if args.json:
        print(json.dumps(inventory, indent=2))
        return 0
    print(f"Report: {inventory['report']}")
    print(f"SHA-256: {inventory['sha256']}")
    print(f"Matched data: {inventory['matched_data']:,} / {inventory['total_data']:,} bytes")
    print(f"Source-selected data: {inventory['complete_data']:,} / {inventory['total_data']:,} bytes")
    print(f"Lost credit: {inventory['unmatched_data']:,} bytes across {inventory['units_with_data_gap']} units")
    print("Section sizes reconcile exactly with reported lost credit.")
    print("\nSection totals:")
    for section in inventory["sections"]:
        print(f"{section['lost_credit']:>10,} bytes  {section['section_count']:>4} sections  {section['name']}")
    print("\nUnits ranked by lost credit:")
    for unit in inventory["units"][:args.limit or None]:
        print(f"{unit['lost_credit']:>10,} bytes  {unit['name']}")
        for section in unit["sections"]:
            score = section["fuzzy_match_percent"]
            display = "missing" if score is None else f"{score:.6f}%"
            print(f"  {section['size']:>8,} bytes  {section['name']:<12} {display}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
