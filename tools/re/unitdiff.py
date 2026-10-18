import argparse
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
EXE = ".exe" if os.name == "nt" else ""
MARKS = {"DIFF_REPLACE": "r", "DIFF_DELETE": "<", "DIFF_INSERT": ">", "DIFF_ARG_MISMATCH": "|", "DIFF_OP_MISMATCH": "o"}


def find_tool(name, candidates):
    for candidate in candidates:
        path = REPO_ROOT / candidate
        if path.is_file():
            return path
    found = shutil.which(name)
    if found:
        return Path(found)
    raise ValueError(f"{name} not found; follow docs/SETUP.md")


def load_units():
    path = REPO_ROOT / "objdiff.json"
    if not path.is_file():
        raise ValueError("objdiff.json is missing; run python configure.py first")
    return json.loads(path.read_text(encoding="utf-8"))["units"]


def normalize(text):
    path = Path(text)
    if path.is_absolute():
        try:
            path = path.resolve().relative_to(REPO_ROOT)
        except ValueError:
            pass
    return path.as_posix().lower()


def resolve_unit(units, query):
    wanted = normalize(query)
    for unit in units:
        source = (unit.get("metadata") or {}).get("source_path", "")
        if unit["name"].lower() in (wanted, f"main/{wanted}") or (source and source.lower() == wanted):
            return unit
    matches = [unit for unit in units if unit["name"].lower().endswith(f"/{wanted}") or Path(unit["name"]).name.lower() == Path(wanted).stem]
    if len(matches) == 1:
        return matches[0]
    if matches:
        raise ValueError(f"{query!r} is ambiguous: {', '.join(unit['name'] for unit in matches)}")
    raise ValueError(f"No objdiff unit matches {query!r}")


def build_objects(unit):
    if not unit.get("base_path"):
        raise ValueError(f"{unit['name']} has no source object; register it in configure.py first")
    ninja = find_tool("ninja", [Path(".venv") / "Scripts" / f"ninja{EXE}", Path(".venv") / "bin" / "ninja"])
    result = subprocess.run([str(ninja), unit["base_path"]], cwd=REPO_ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if result.returncode != 0:
        sys.stdout.write(result.stdout)
        sys.stderr.write(result.stderr)
        raise RuntimeError(f"ninja exited {result.returncode} building {unit['base_path']}")
    if not (REPO_ROOT / unit["target_path"]).is_file():
        raise ValueError(f"{unit['target_path']} is missing; run a full ninja build once to split the original")


def run_objdiff(unit):
    objdiff = find_tool("objdiff-cli", [Path("tools") / "local" / f"objdiff-cli{EXE}", Path("build") / "tools" / f"objdiff-cli{EXE}"])
    result = subprocess.run([str(objdiff), "diff", "-p", ".", "-u", unit["name"], "-o", "-", "--format", "json"], cwd=REPO_ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if result.returncode != 0:
        raise RuntimeError(f"objdiff-cli exited {result.returncode}: {result.stderr.strip()}")
    return result.stdout


def number(value):
    return int(value or 0)


def percent(value):
    return "missing" if value is None else f"{value:.2f}%"


def instruction_text(row):
    return (row.get("instruction") or {}).get("formatted", "")


def instruction_offset(row, symbol):
    instruction = row.get("instruction")
    if not instruction:
        return None
    return number(instruction.get("address")) - number(symbol.get("address"))


def diff_rows(left, right):
    left_rows = left.get("instructions") or []
    right_rows = (right or {}).get("instructions") or []
    rows = []
    for index in range(max(len(left_rows), len(right_rows))):
        target = left_rows[index] if index < len(left_rows) else {}
        current = right_rows[index] if index < len(right_rows) else {}
        kind = target.get("diff_kind") or current.get("diff_kind") or ""
        offset = instruction_offset(target, left)
        if offset is None and right:
            offset = instruction_offset(current, right)
        rows.append((offset, MARKS.get(kind, " "), instruction_text(target), instruction_text(current)))
    return rows


def print_rows(rows, limit, indent):
    shown = [row for row in rows if row[1] != " "] if limit else rows
    for offset, mark, target, current in shown[:limit] if limit else shown:
        where = "" if offset is None else f"{offset:#06x}"
        print(f"{indent}{where:>7} {mark} {target:<40} {current}")
    if limit and len(shown) > limit:
        print(f"{indent}        ... {len(shown) - limit} more differing row(s); use --function for the full diff")


def section_rows(diff):
    right = {section["name"]: section for section in diff["right"].get("sections", [])}
    for section in diff["left"].get("sections", []):
        if section.get("kind") not in ("SECTION_CODE", "SECTION_DATA", "SECTION_BSS"):
            continue
        current = right.get(section["name"])
        yield section["name"], number(section.get("size")), number((current or {}).get("size")) if current else None, section.get("match_percent")


def report(unit, diff, args):
    left_symbols = diff["left"].get("symbols", [])
    right_symbols = diff["right"].get("symbols", [])
    functions = [symbol for symbol in left_symbols if symbol.get("kind") == "SYMBOL_FUNCTION" and not symbol["name"].startswith("gap_")]
    if args.function:
        functions = [symbol for symbol in functions if args.function in (symbol["name"], symbol.get("demangled_name"))]
        if not functions:
            raise ValueError(f"No function {args.function!r} in {unit['name']}")
    missing = [symbol for symbol in functions if symbol.get("match_percent") is None]
    shown = functions if args.function or args.all else [symbol for symbol in functions if symbol.get("match_percent") is not None]
    width = min(max([len(symbol["name"]) for symbol in shown] + [8]), 48)
    source = (unit.get("metadata") or {}).get("source_path", "")
    print(f"{unit['name']}  {source}")
    print(f"  {'section':<12}{'target':>8}{'current':>9}  match")
    for name, size, current, match in section_rows(diff):
        print(f"  {name:<12}{size:>8}{'-' if current is None else current:>9}  {percent(match)}")
    print(f"  {'function':<{width}}  {'size':>6}  match")
    for symbol in shown:
        match = symbol.get("match_percent")
        partner = right_symbols[symbol["target_symbol"]] if symbol.get("target_symbol") is not None else None
        print(f"  {symbol['name']:<{width}}  {number(symbol.get('size')):>6}  {percent(match)}")
        if args.function:
            print_rows(diff_rows(symbol, partner), 0, "    ")
        elif partner and match is not None and match < 100.0:
            print_rows(diff_rows(symbol, partner), args.rows, "    ")
    if args.function:
        return
    if missing and not args.all:
        print(f"  {len(missing)} target function(s) not in source ({sum(number(symbol.get('size')) for symbol in missing)} bytes); --all lists them")
    extras = [symbol["name"] for symbol in right_symbols if symbol.get("kind") == "SYMBOL_FUNCTION" and symbol.get("target_symbol") is None]
    if extras:
        print(f"  only in source: {', '.join(extras)}")
    perfect = [symbol for symbol in functions if symbol.get("match_percent") == 100.0]
    total = sum(number(symbol.get("size")) for symbol in functions)
    print(f"  {len(perfect)}/{len(functions)} functions at 100%, {sum(number(symbol.get('size')) for symbol in perfect)}/{total} code bytes")


def main():
    parser = argparse.ArgumentParser(description="Build one unit's object and diff it against the original with objdiff-cli, without linking.")
    parser.add_argument("unit", help="objdiff unit name (main/game/BController, game/BController, BController) or source path")
    parser.add_argument("--function", help="print the full instruction diff of one function")
    parser.add_argument("--rows", type=int, default=6, help="differing rows to show per function (default 6)")
    parser.add_argument("--all", action="store_true", help="also list target functions missing from the source")
    parser.add_argument("--json", action="store_true", help="print objdiff-cli JSON unchanged")
    parser.add_argument("--no-build", action="store_true", help="diff the existing object without running ninja")
    args = parser.parse_args()
    started = time.perf_counter()
    try:
        unit = resolve_unit(load_units(), args.unit)
        if not args.no_build:
            build_objects(unit)
        output = run_objdiff(unit)
        if args.json:
            sys.stdout.write(output)
            return 0
        report(unit, json.loads(output), args)
    except (OSError, ValueError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    print(f"  {time.perf_counter() - started:.2f}s")
    return 0


if __name__ == "__main__":
    sys.exit(main())
