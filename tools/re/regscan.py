import argparse
import difflib
import json
import re
import subprocess
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
OBJDUMP = REPO_ROOT / "tools" / "local" / "binutils" / "powerpc-eabi-objdump.exe"
BUILD = REPO_ROOT / "build" / "wpse-v1"


def functions(obj):
    text = subprocess.run([str(OBJDUMP), "-d", "-r", str(obj)], capture_output=True, text=True).stdout
    result = {}
    current = None
    for line in text.splitlines():
        header = re.match(r"^[0-9a-f]+ <(.+)>:$", line)
        if header:
            current = header.group(1)
            result[current] = []
            continue
        if current is None:
            continue
        insn = re.match(r"^\s+[0-9a-f]+:\s+(?:[0-9a-f]{2} ){4}\s*(.*)$", line)
        if insn:
            text_insn = re.sub(r"\s+", " ", insn.group(1).strip())
            text_insn = re.sub(r"(b\w*) (cr\d,)?[0-9a-f]+ <[^>]*>", r"\1 \2L", text_insn)
            result[current].append(text_insn)
    return result


def masked(insn):
    return re.sub(r"\b[rf]\d+\b", "R", insn)


def main():
    parser = argparse.ArgumentParser(description="Split near-miss functions into register-only and source differences.")
    parser.add_argument("--min", type=float, default=95.0, help="lowest fuzzy match percent to include")
    parser.add_argument("--unit", default="", help="only units whose name contains this text")
    args = parser.parse_args()
    report = json.loads((BUILD / "report.json").read_text(encoding="utf-8"))
    rows = []
    for unit in report["units"]:
        name = unit["name"]
        if not name.startswith("main/") or args.unit not in name:
            continue
        candidates = [f for f in unit.get("functions", []) if args.min <= f.get("fuzzy_match_percent", 0) < 100]
        if not candidates:
            continue
        target = BUILD / "obj" / (name[5:] + ".o")
        ours = BUILD / "src" / (name[5:] + ".o")
        if not (target.exists() and ours.exists()):
            continue
        a = functions(target)
        b = functions(ours)
        for function in candidates:
            symbol = function["name"]
            if symbol not in a or symbol not in b:
                continue
            x = [masked(i) for i in a[symbol]]
            y = [masked(i) for i in b[symbol]]
            matcher = difflib.SequenceMatcher(None, x, y, autojunk=False)
            source = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in matcher.get_opcodes() if tag != "equal")
            registers = sum(1 for i, j in zip(a[symbol], b[symbol]) if i != j)
            rows.append((source, registers, function["fuzzy_match_percent"], name[5:], symbol))
    rows.sort()
    for source, registers, percent, unit, symbol in rows:
        print("%4d source %4d register %6.2f%%  %s  %s" % (source, registers, percent, unit, symbol))


if __name__ == "__main__":
    main()
