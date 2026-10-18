import argparse
import re
import subprocess
import sys
from pathlib import Path


DRIVER = "decomp-config"
SYMBOL_LINE = re.compile(r"^(?P<name>\S+) = (?P<section>[^:\s]+):(?P<address>0x[0-9A-Fa-f]+);")
OBJECT_LINE = re.compile(r'^\s*Object\(\s*\w+\s*,\s*"(?P<path>[^"]+)"')
PLACEHOLDER = re.compile(r"^(fn|lbl|func|dtor|sinit|jumptable|switch|gap|@)[_\d]")


def read(path):
    data = Path(path).read_bytes().decode("utf-8")
    newline = "\r\n" if "\r\n" in data else "\n"
    return data.replace("\r\n", "\n"), newline


def write(path, text, newline):
    Path(path).write_bytes(text.replace("\n", newline).encode("utf-8"))


def ordered_union(ours, theirs):
    result = list(ours)
    seen = set(result)
    for index, key in enumerate(theirs):
        if key in seen:
            continue
        anchor = next((theirs[back] for back in range(index - 1, -1, -1) if theirs[back] in seen), None)
        position = result.index(anchor) + 1 if anchor is not None else 0
        result.insert(position, key)
        seen.add(key)
    return result


def pick(base, ours, theirs):
    if ours == theirs:
        return ours, False
    if ours == base:
        return theirs, False
    if theirs == base:
        return ours, False
    return None, True


def symbol_key(line):
    match = SYMBOL_LINE.match(line)
    return (match["section"], int(match["address"], 16)) if match else None


def symbol_name(line):
    match = SYMBOL_LINE.match(line)
    return match["name"] if match else ""


def resolve_symbol(base, ours, theirs):
    if ours is None or theirs is None:
        return None
    ours_rest = ours.split(";", 1)[1]
    theirs_rest = theirs.split(";", 1)[1]
    base_rest = base.split(";", 1)[1] if base else None
    ours_named = not PLACEHOLDER.match(symbol_name(ours))
    theirs_named = not PLACEHOLDER.match(symbol_name(theirs))
    rest, conflict = pick(base_rest, ours_rest, theirs_rest)
    if conflict:
        return None
    if symbol_name(ours) == symbol_name(theirs):
        name = symbol_name(ours)
    elif base and symbol_name(ours) == symbol_name(base):
        name = symbol_name(theirs)
    elif base and symbol_name(theirs) == symbol_name(base):
        name = symbol_name(ours)
    elif ours_named and not theirs_named:
        name = symbol_name(ours)
    elif theirs_named and not ours_named:
        name = symbol_name(theirs)
    else:
        return None
    head = (ours if symbol_name(ours) == name else theirs).split(";", 1)[0]
    return f"{head};{rest}"


def keyed_lines(text):
    entries = {}
    order = []
    counts = {}
    for line in text.split("\n"):
        address = symbol_key(line)
        if address is None:
            if line.strip():
                raise ValueError(f"Unrecognized symbols.txt line: {line}")
            continue
        key = (*address, counts.get(address, 0))
        counts[address] = key[2] + 1
        entries[key] = line
        order.append(key)
    return entries, order


def merge_symbols(base_text, ours_text, theirs_text):
    base, _ = keyed_lines(base_text)
    ours, ours_order = keyed_lines(ours_text)
    theirs, theirs_order = keyed_lines(theirs_text)
    lines = []
    conflicts = 0
    for key in ordered_union(ours_order, theirs_order):
        chosen, conflict = pick(base.get(key), ours.get(key), theirs.get(key))
        if conflict:
            chosen = resolve_symbol(base.get(key), ours.get(key), theirs.get(key))
        if chosen is None and (conflict or key not in base):
            if ours.get(key) is None or theirs.get(key) is None:
                chosen = ours.get(key) or theirs.get(key)
            else:
                conflicts += 1
                lines += ["<<<<<<< ours", ours[key], "=======", theirs[key], ">>>>>>> theirs"]
                continue
        if chosen is not None:
            lines.append(chosen)
    return "\n".join(lines) + "\n", conflicts


def split_blocks(text):
    chunks = [chunk for chunk in re.split(r"\n\s*\n", text.strip("\n")) if chunk.strip()]
    header = chunks[0]
    if not header.startswith("Sections:"):
        raise ValueError("splits.txt does not start with a Sections block")
    blocks = {}
    order = []
    for chunk in chunks[1:]:
        name = chunk.split("\n", 1)[0]
        if not name.endswith(":") or name.startswith("\t"):
            raise ValueError(f"Unrecognized splits.txt block: {name}")
        blocks[name] = chunk
        order.append(name)
    return header, blocks, order


def block_address(chunk):
    starts = dict(re.findall(r"^	(\S+)\s+start:(0x[0-9A-Fa-f]+)", chunk, re.M))
    start = starts.get(".text") or starts.get(".init") or (min(starts.values(), key=lambda value: int(value, 16)) if starts else "0x0")
    return int(start, 16)


def address_union(ours_order, theirs_order, chunk_of):
    result = list(ours_order)
    seen = set(result)
    for key in theirs_order:
        if key in seen:
            continue
        address = block_address(chunk_of(key))
        position = next((index for index, other in enumerate(result) if block_address(chunk_of(other)) > address), len(result))
        result.insert(position, key)
        seen.add(key)
    return result


def merge_splits(base_text, ours_text, theirs_text):
    base_header, base, _ = split_blocks(base_text)
    ours_header, ours, ours_order = split_blocks(ours_text)
    theirs_header, theirs, theirs_order = split_blocks(theirs_text)
    conflicts = 0
    header, conflict = pick(base_header, ours_header, theirs_header)
    if conflict:
        conflicts += 1
        header = f"<<<<<<< ours\n{ours_header}\n=======\n{theirs_header}\n>>>>>>> theirs"
    chunks = [header]
    for name in address_union(ours_order, theirs_order, lambda name: ours.get(name) or theirs[name]):
        chosen, conflict = pick(base.get(name), ours.get(name), theirs.get(name))
        if conflict:
            if ours.get(name) is None or theirs.get(name) is None:
                chosen = ours.get(name) or theirs.get(name)
            else:
                conflicts += 1
                chosen = f"<<<<<<< ours\n{ours[name]}\n=======\n{theirs[name]}\n>>>>>>> theirs"
        if chosen is not None:
            chunks.append(chosen)
    return "\n\n".join(chunks) + "\n", conflicts


def diff3(base_path, ours_path, theirs_path):
    result = subprocess.run(["git", "merge-file", "-p", "--diff3", "-L", "ours", "-L", "base", "-L", "theirs", ours_path, base_path, theirs_path], capture_output=True)
    return result.stdout.decode("utf-8").replace("\r\n", "\n")


def object_key(line):
    match = OBJECT_LINE.match(line)
    return match["path"] if match else None


def ledger_key(line):
    if not line.startswith("| ") or line.startswith("| ---") or line.startswith("| Unit |"):
        return None
    cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
    if len(cells) < 3:
        return None
    return cells[2] if "src/" in cells[2] else cells[0]


def resolve_hunk(base, ours, theirs, key_of=object_key):
    lines = base + ours + theirs
    if lines and all(key_of(line) for line in lines if line.strip()):
        base_map = {key_of(line): line for line in base if line.strip()}
        ours_map = {key_of(line): line for line in ours if line.strip()}
        theirs_map = {key_of(line): line for line in theirs if line.strip()}
        merged = []
        for key in ordered_union([key_of(line) for line in ours if line.strip()], [key_of(line) for line in theirs if line.strip()]):
            chosen, conflict = pick(base_map.get(key), ours_map.get(key), theirs_map.get(key))
            if conflict:
                if ours_map.get(key) is None or theirs_map.get(key) is None:
                    chosen = ours_map.get(key) or theirs_map.get(key)
                else:
                    return None
            if chosen is not None:
                merged.append(chosen)
        return merged
    if not any(line.strip() for line in base):
        return ours + [line for line in theirs if line not in ours]
    return None


def merge_lines(base_path, ours_path, theirs_path, key_of=object_key, union=False):
    text = diff3(base_path, ours_path, theirs_path)
    output = []
    conflicts = 0
    state = None
    hunk = {"ours": [], "base": [], "theirs": []}
    for line in text.split("\n"):
        if line.startswith("<<<<<<< ours"):
            state = "ours"
            hunk = {"ours": [], "base": [], "theirs": []}
        elif line.startswith("||||||| base") and state == "ours":
            state = "base"
        elif line.startswith("=======") and state == "base":
            state = "theirs"
        elif line.startswith(">>>>>>> theirs") and state == "theirs":
            blank = any(not line.strip() for line in hunk["base"] + hunk["ours"] + hunk["theirs"])
            resolved = None if union and blank else resolve_hunk(hunk["base"], hunk["ours"], hunk["theirs"], key_of)
            if resolved is None and union:
                resolved = hunk["ours"] + [line for line in hunk["theirs"] if line not in hunk["ours"]]
            if resolved is None:
                conflicts += 1
                output += ["<<<<<<< ours", *hunk["ours"], "=======", *hunk["theirs"], ">>>>>>> theirs"]
            else:
                output += resolved
            state = None
        elif state:
            hunk[state].append(line)
        else:
            output.append(line)
    return "\n".join(output), conflicts


def main():
    parser = argparse.ArgumentParser(description="Git merge driver for splits.txt, symbols.txt, configure.py and ledger tables that merges independent entries automatically.")
    parser.add_argument("base")
    parser.add_argument("ours")
    parser.add_argument("theirs")
    parser.add_argument("path")
    args = parser.parse_args()
    try:
        base_text, _ = read(args.base)
        ours_text, newline = read(args.ours)
        theirs_text, _ = read(args.theirs)
        name = Path(args.path).name
        if name == "symbols.txt":
            merged, conflicts = merge_symbols(base_text, ours_text, theirs_text)
        elif name == "splits.txt":
            merged, conflicts = merge_splits(base_text, ours_text, theirs_text)
        elif Path(args.path).parent.name == "ledger":
            merged, conflicts = merge_lines(args.base, args.ours, args.theirs, ledger_key, union=True)
        else:
            merged, conflicts = merge_lines(args.base, args.ours, args.theirs)
    except (OSError, UnicodeDecodeError, ValueError) as error:
        print(f"{DRIVER}: {args.path}: {error}; falling back to a text merge", file=sys.stderr)
        union = ["--union"] if Path(args.path).parent.name == "ledger" else []
        return subprocess.run(["git", "merge-file", *union, "-L", "ours", "-L", "base", "-L", "theirs", args.ours, args.base, args.theirs]).returncode
    write(args.ours, merged, newline)
    if conflicts:
        print(f"{DRIVER}: {args.path}: {conflicts} entr{'y' if conflicts == 1 else 'ies'} changed differently on both sides; resolve the markers by hand", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
