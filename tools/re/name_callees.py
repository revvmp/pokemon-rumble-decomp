import argparse
import collections
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
SYMBOLS = os.path.join(ROOT, "config", "wpse-v1", "symbols.txt")
NM = os.path.join(ROOT, "tools", "local", "binutils", "powerpc-eabi-nm.exe")
PLACEHOLDER = re.compile(r"(fn_|dtor_)[0-9A-F]{8}$")
ROW = re.compile(r"\s+0x[0-9a-f]+ \| (?:bl|b) (\S+)\s+(?:bl|b) (\S+)")
SYMBOL = re.compile(r"^(\S+) = \.(\w+):0x([0-9A-F]+); // (.*)$")


def object_sizes():
    sizes = {}
    for path in glob.glob(os.path.join(ROOT, "build", "wpse-v1", "src", "game", "*.o")):
        out = subprocess.run([NM, "-S", "--defined-only", path], capture_output=True, text=True).stdout
        for line in out.split("\n"):
            parts = line.split()
            if len(parts) == 4:
                sizes.setdefault(parts[3], set()).add(int(parts[1], 16))
    return sizes


def symbol_sizes():
    table = {}
    for line in open(SYMBOLS):
        match = SYMBOL.match(line.strip())
        if match:
            size = re.search(r"size:0x([0-9A-F]+)", match.group(4))
            table.setdefault(match.group(1), []).append(int(size.group(1), 16) if size else 0)
    return table


def collect_pairs(placeholders_only):
    ours = object_sizes()
    table = symbol_sizes()
    pairs = collections.defaultdict(collections.Counter)
    units = [os.path.basename(p)[:-2] for p in sorted(glob.glob(os.path.join(ROOT, "build", "wpse-v1", "src", "game", "*.o")))]
    for unit in units:
        out = subprocess.run(
            [sys.executable, os.path.join(ROOT, "tools", "re", "unitdiff.py"), unit, "--rows", "2000", "--no-build"],
            capture_output=True, text=True, cwd=ROOT).stdout
        for line in out.split("\n"):
            match = ROW.match(line)
            if not match:
                continue
            target, mine = match.groups()
            if target == mine or mine not in ours:
                continue
            if placeholders_only:
                if PLACEHOLDER.match(target) and not PLACEHOLDER.match(mine):
                    pairs[target][mine] += 1
            elif target in table and target not in ours and mine not in table:
                pairs[target][mine] += 1
    chosen = []
    for target, counter in pairs.items():
        if len(counter) != 1 or len(table.get(target, [])) != 1:
            continue
        mine = next(iter(counter))
        if table[target][0] in ours[mine]:
            chosen.append((target, mine))
    return chosen


def apply(chosen):
    lines = open(SYMBOLS).read().split("\n")
    taken = collections.Counter()
    for line in lines:
        match = re.match(r"^(\S+) = ", line)
        if match:
            taken[match.group(1)] += 1
    wanted = collections.Counter(mine for _, mine in chosen)
    renames = dict(chosen)
    done = 0
    for index, line in enumerate(lines):
        match = re.match(r"^(\S+) = (.*)$", line)
        if match and match.group(1) in renames:
            mine = renames[match.group(1)]
            rest = match.group(2)
            if (taken[mine] > 0 or wanted[mine] > 1) and "scope:local" not in rest:
                rest = rest.rstrip() + " scope:local"
            lines[index] = mine + " = " + rest
            done += 1
    with open(SYMBOLS, "w") as handle:
        handle.write("\n".join(lines))
    return done


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--placeholders", action="store_true")
    parser.add_argument("--apply", action="store_true")
    args = parser.parse_args()
    chosen = collect_pairs(args.placeholders)
    for target, mine in chosen:
        print(target[:60], "->", mine[:90])
    print(len(chosen), "renames")
    if args.apply:
        print(apply(chosen), "applied")


if __name__ == "__main__":
    main()
