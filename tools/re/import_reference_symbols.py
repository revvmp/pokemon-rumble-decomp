import argparse
import bisect
import json
import math
import re
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SYMBOLS = REPO_ROOT / "config" / "wpse-v1" / "symbols.txt"

REFERENCES = [
    {"label": "xenoblade-jp", "repo": "xbret/xenoblade", "commit": "ea25691b2029394ab777bda7060139e517a13cba", "path": "config/jp", "format": "dtk", "license": "CC0-1.0"},
    {"label": "ss-SOUE01", "repo": "zeldaret/ss", "commit": "787b0b1e9d30e3c1d91ced61350b059ffc5a2411", "path": "config/SOUE01", "format": "dtk", "license": "CC0-1.0"},
    {"label": "tp-RZDE01_02", "repo": "zeldaret/tp", "commit": "c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0", "path": "config/RZDE01_02", "format": "dtk", "license": "CC0-1.0"},
    {"label": "kdc-S72E01", "repo": "Swiftshine/kdc", "commit": "81b52d6a31b6dd671bea0268f36a874477ee2bf6", "path": "config/S72E01", "format": "dtk", "license": "CC0-1.0"},
    {"label": "ogws-RSPE01_01", "repo": "kiwi515/ogws", "commit": "27fa94a593096eed7442efc4e70981fd52025b9c", "path": "config/RSPE01_01", "format": "dtk", "license": "CC0-1.0"},
    {"label": "petari-RMGE01", "repo": "SMGCommunity/Petari", "commit": "e0ab141914921726a595573dc48d3f13d0922efe", "path": "config/RMGE01", "format": "dtk", "license": "CC0-1.0"},
    {"label": "wii-symbols", "repo": "riptl/wii-symbols", "commit": "34d03db117a7e0007ca3ef3c8bc7402dad1c2925", "path": "symbols", "format": "map", "license": "none stated; names only"},
]

LIBRARIES = [
    ("runtime-init", 0x80004000, 0x800046E4, "msl"),
    ("revoex-ncd", 0x801770F4, 0x801778F0, "revoex"),
    ("sdk-cnt", 0x801778F0, 0x80178530, "sdk"),
    ("hbm", 0x80178530, 0x8019DC50, "hbm"),
    ("nw4r-ut-cntfilestream", 0x801AF870, 0x801AFC80, "nw4r"),
    ("msl-printf", 0x801E8DE0, 0x801EE470, "msl"),
    ("tmc-jpeg", 0x801F6060, 0x801FB030, "middleware"),
    ("fa", 0x801FB030, 0x80225820, "middleware"),
    ("nw4r-ef", 0x80225820, 0x8024AE80, "nw4r"),
    ("nw4r-g3d", 0x8024AE80, 0x80271E10, "nw4r"),
    ("nw4r-snd", 0x80271E10, 0x80292D00, "nw4r"),
    ("nw4r-ut-lyt-math-db", 0x80292D00, 0x802B1800, "nw4r"),
    ("sdk", 0x802B1800, 0x8033F974, "sdk"),
    ("msl-runtime-trk", 0x8033F974, 0x803529C0, "msl"),
]

FAMILY_PATTERNS = {
    "sdk": re.compile(r"(revolution|RVL_SDK|rvl_sdk|/bte/|NdevExi2A|ndev)", re.I),
    "msl": re.compile(r"(MSL|Runtime|MetroTRK|TRK_MINNOW|PowerPC_EABI)", re.I),
    "nw4r": re.compile(r"nw4r/", re.I),
    "hbm": re.compile(r"(hbm|homebutton)", re.I),
    "revoex": re.compile(r"(RevoEX|/ncd/|/net/|NWC24)", re.I),
    "middleware": re.compile(r"$^"),
}
FAMILY_EXCLUDE = {
    "sdk": re.compile(r"(hbm|homebutton)", re.I),
    "nw4r": re.compile(r"nw4hbm", re.I),
}

SYMBOL_LINE = re.compile(r"^(?P<name>\S+) = (?P<section>[^:]+):0x(?P<addr>[0-9A-Fa-f]+);(?P<rest>.*)$")
SIZE_ATTR = re.compile(r"size:0x([0-9A-Fa-f]+)")
TYPE_ATTR = re.compile(r"type:(\w+)")
PLACEHOLDER = re.compile(r"^(fn|lbl|dtor)_[0-9A-Fa-f]{8}$")
REF_PLACEHOLDER = re.compile(r"^[A-Za-z]+_[0-9A-Fa-f]{6,8}$|^gap_|^@\d+$|\$|\.\.\.")
MAP_LINE = re.compile(r"^pos=([0-9A-Fa-f]+) len=(\d+) sym=(\S+)")
CODE_SECTIONS = (".init", ".text")
DATA_SECTIONS = (".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2")
SEED_BITS = 30.0
GAP_SLACK = 3
CONTIGUOUS_GAP = 0x20
AMBIGUITY_SPAN = 64
MANGLED = re.compile(r"__(F|Q\d|C|\d)")
MAP_NAME_FILTERS = {
    "sdk": lambda name: not MANGLED.search(name) and "nw4" not in name and "homebutton" not in name,
    "msl": lambda name: "nw4" not in name and "homebutton" not in name,
    "nw4r": lambda name: "nw4r" in name,
    "hbm": lambda name: "nw4hbm" in name or "homebutton" in name or name.startswith(("HBM", "__HBM")),
    "revoex": lambda name: not MANGLED.search(name) and "nw4" not in name,
    "middleware": lambda name: not MANGLED.search(name) and "nw4" not in name,
}


def parse_symbols(path):
    symbols = []
    for number, line in enumerate(path.read_text(encoding="utf-8").splitlines()):
        match = SYMBOL_LINE.match(line)
        if not match:
            continue
        rest = match["rest"]
        size = SIZE_ATTR.search(rest)
        kind = TYPE_ATTR.search(rest)
        symbols.append({
            "line": number,
            "name": match["name"],
            "section": match["section"],
            "addr": int(match["addr"], 16),
            "size": int(size[1], 16) if size else 0,
            "type": kind[1] if kind else "",
        })
    return symbols


def parse_splits(path):
    units = []
    current = None
    for line in path.read_text(encoding="utf-8").splitlines():
        if line and not line[0].isspace() and line.endswith(":") and line != "Sections:":
            current = line[:-1]
            continue
        if current is None:
            continue
        fields = line.split()
        if len(fields) >= 3 and fields[1].startswith("start:") and fields[2].startswith("end:"):
            units.append((current, fields[0], int(fields[1][6:], 16), int(fields[2][4:], 16)))
    return units


def load_dtk_reference(directory, label, sections):
    symbols = parse_symbols(directory / "symbols.txt")
    units = parse_splits(directory / "splits.txt")
    by_section = defaultdict(list)
    for path, section, start, end in units:
        by_section[section].append((start, end, path))
    for spans in by_section.values():
        spans.sort()
    entries = []
    for symbol in symbols:
        if symbol["section"] not in sections or symbol["size"] == 0:
            continue
        if sections is CODE_SECTIONS and symbol["type"] != "function":
            continue
        if sections is DATA_SECTIONS and symbol["type"] != "object":
            continue
        unit = None
        for start, end, path in by_section.get(symbol["section"], ()):
            if start <= symbol["addr"] < end:
                unit = path
                break
        entries.append({"name": symbol["name"], "section": symbol["section"], "addr": symbol["addr"], "size": symbol["size"], "unit": unit, "ref": label})
    entries.sort(key=lambda entry: (entry["section"], entry["addr"]))
    return entries


def load_map_reference(path, label):
    entries = []
    for line in path.read_text(encoding="utf-8").splitlines():
        match = MAP_LINE.match(line)
        if match and int(match[2]) > 0:
            entries.append({"name": match[3], "section": ".text", "addr": int(match[1], 16), "size": int(match[2]), "unit": None, "ref": label})
    entries.sort(key=lambda entry: entry["addr"])
    unique = []
    for entry in entries:
        if unique and unique[-1]["addr"] == entry["addr"]:
            continue
        unique.append(entry)
    return unique


def reference_sets(refs_dir, sections):
    loaded = []
    for reference in REFERENCES:
        base = refs_dir / reference["repo"].replace("/", "_") / reference["path"]
        if reference["format"] == "dtk":
            if (base / "symbols.txt").exists():
                loaded.append((reference, reference["label"], load_dtk_reference(base, reference["label"], sections)))
        elif sections is CODE_SECTIONS and base.is_dir():
            for map_path in sorted(base.glob("*.txt")):
                label = f"{reference['label']}-{map_path.stem}"
                loaded.append((reference, label, load_map_reference(map_path, label)))
    return loaded


def family_entries(entries, family):
    pattern = FAMILY_PATTERNS[family]
    exclude = FAMILY_EXCLUDE.get(family)
    chosen = []
    name_filter = MAP_NAME_FILTERS[family]
    for entry in entries:
        if entry["unit"] is None:
            if name_filter(entry["name"]):
                chosen.append(entry)
            continue
        if pattern.search(entry["unit"]) and not (exclude and exclude.search(entry["unit"])):
            chosen.append(entry)
    return chosen


def contiguous(previous, entry):
    if previous["section"] != entry["section"]:
        return False
    if previous["unit"] != entry["unit"]:
        return False
    gap = entry["addr"] - (previous["addr"] + previous["size"])
    return 0 <= gap < CONTIGUOUS_GAP


def size_bits(ours):
    counts = Counter(symbol["size"] for symbol in ours)
    total = len(ours)
    return lambda size: -math.log2((counts.get(size, 0) + 1) / (total + 1))


def find_runs(ours, ref, bits):
    index = defaultdict(list)
    for j, entry in enumerate(ref):
        index[entry["size"]].append(j)
    runs = []
    for i, symbol in enumerate(ours):
        for j in index.get(symbol["size"], ()):
            if i > 0 and j > 0 and ours[i - 1]["size"] == ref[j - 1]["size"] and contiguous(ref[j - 1], ref[j]):
                continue
            length = 1
            while i + length < len(ours) and j + length < len(ref) and ours[i + length]["size"] == ref[j + length]["size"] and contiguous(ref[j + length - 1], ref[j + length]):
                length += 1
            if length >= 3:
                score = sum(bits(size) for size in {ours[i + k]["size"] for k in range(length)})
                runs.append((score, length, i, j))
    by_ours = sorted(runs, key=lambda run: run[2])
    by_ref = sorted(runs, key=lambda run: run[3])
    ours_starts = [run[2] for run in by_ours]
    ref_starts = [run[3] for run in by_ref]
    longest = max((run[1] for run in runs), default=0)
    kept = []
    for run in runs:
        score, length, i, j = run
        nearby = by_ours[bisect.bisect_left(ours_starts, i - longest):bisect.bisect_left(ours_starts, i + length)]
        nearby += by_ref[bisect.bisect_left(ref_starts, j - longest):bisect.bisect_left(ref_starts, j + length)]
        if not ambiguous(run, nearby):
            kept.append(run)
    return kept


def ambiguous(run, runs):
    score, length, i, j = run
    for other_score, other_length, other_i, other_j in runs:
        if other_j - other_i == j - i or abs((other_j - other_i) - (j - i)) > AMBIGUITY_SPAN:
            continue
        if other_score < score * 0.5:
            continue
        if other_i < i + length and i < other_i + other_length:
            return True
        if other_j < j + length and j < other_j + other_length:
            return True
    return False


def select_runs(runs, threshold):
    used_ours = set()
    used_ref = set()
    chosen = []
    for score, length, i, j in sorted(runs, reverse=True):
        if score < threshold:
            break
        if any(i + k in used_ours or j + k in used_ref for k in range(length)):
            continue
        used_ours.update(range(i, i + length))
        used_ref.update(range(j, j + length))
        chosen.append((score, length, i, j))
    return chosen, used_ours, used_ref


def fill_window(ours_slice, ref_slice, bits, closed):
    pairs = []
    if not ours_slice or not ref_slice:
        return pairs
    ours_sizes = [ours[1]["size"] for ours in ours_slice]
    ref_sizes = [ref[1]["size"] for ref in ref_slice]
    taken_ours = set()
    taken_ref = set()
    for a in range(len(ours_sizes)):
        for b in range(len(ref_sizes)):
            length = 0
            while a + length < len(ours_sizes) and b + length < len(ref_sizes) and ours_sizes[a + length] == ref_sizes[b + length]:
                length += 1
            window = tuple(ours_sizes[a:a + length])
            if length >= 3 and occurrences(ours_sizes, window) == 1 and occurrences(ref_sizes, window) == 1 and len(set(window)) > 1 and not any(a + k in taken_ours or b + k in taken_ref for k in range(length)):
                for k in range(length):
                    taken_ours.add(a + k)
                    taken_ref.add(b + k)
                    pairs.append((ours_slice[a + k][0], ref_slice[b + k][0], "run", length))
    ours_counts = Counter(ours_sizes)
    ref_counts = Counter(ref_sizes)
    for a, size in enumerate(ours_sizes):
        if a in taken_ours or ours_counts[size] != 1 or ref_counts[size] != 1:
            continue
        b = ref_sizes.index(size)
        if not closed or b in taken_ref or (a != b and len(ours_sizes) - a != len(ref_sizes) - b):
            continue
        pairs.append((ours_slice[a][0], ref_slice[b][0], "unique", 1))
    return pairs


def occurrences(sizes, window):
    return sum(1 for start in range(len(sizes) - len(window) + 1) if tuple(sizes[start:start + len(window)]) == window)


def align(ours, ref, bits, threshold):
    runs = find_runs(ours, ref, bits)
    seeds, used_ours, used_ref = select_runs(runs, threshold)
    mapping = {}
    for score, length, i, j in seeds:
        for k in range(length):
            mapping[i + k] = (j + k, "seed", length, score)
    by_unit = defaultdict(list)
    for i, (j, method, length, score) in mapping.items():
        if ref[j]["unit"] is not None:
            by_unit[ref[j]["unit"]].append((j, i))
    for unit, anchors in by_unit.items():
        unit_indices = [j for j, entry in enumerate(ref) if entry["unit"] == unit and entry["section"] == ref[anchors[0][0]]["section"]]
        anchors.sort()
        if any(anchors[k][1] >= anchors[k + 1][1] for k in range(len(anchors) - 1)):
            continue
        first_ref, first_ours = anchors[0]
        last_ref, last_ours = anchors[-1]
        windows = []
        before = [j for j in unit_indices if j < first_ref and j not in used_ref]
        if before:
            low = first_ours - len(before) - GAP_SLACK
            block = trailing_block([i for i in range(max(low, 0), first_ours) if i not in mapping], first_ours)
            windows.append((before, block, False))
        for (ref_a, ours_a), (ref_b, ours_b) in zip(anchors, anchors[1:]):
            gap_ref = [j for j in unit_indices if ref_a < j < ref_b and j not in used_ref]
            gap_ours = [i for i in range(ours_a + 1, ours_b) if i not in mapping]
            closed = len(gap_ours) == ours_b - ours_a - 1 and len(gap_ref) == ref_b - ref_a - 1
            if gap_ref and gap_ours:
                windows.append((gap_ref, gap_ours, closed))
        after = [j for j in unit_indices if j > last_ref and j not in used_ref]
        if after:
            high = last_ours + len(after) + GAP_SLACK
            block = leading_block([i for i in range(last_ours + 1, min(high + 1, len(ours))) if i not in mapping], last_ours)
            windows.append((after, block, False))
        for gap_ref, gap_ours, closed in windows:
            ours_slice = [(i, ours[i]) for i in gap_ours]
            ref_slice = [(j, ref[j]) for j in gap_ref]
            for i, j, method, length in fill_window(ours_slice, ref_slice, bits, closed):
                if i in mapping or j in used_ref:
                    continue
                mapping[i] = (j, method, length, bits(ours[i]["size"]))
                used_ref.add(j)
    return mapping


def trailing_block(indices, anchor):
    block = []
    expected = anchor - 1
    for index in reversed(indices):
        if index != expected:
            break
        block.insert(0, index)
        expected -= 1
    return block


def leading_block(indices, anchor):
    block = []
    expected = anchor + 1
    for index in indices:
        if index != expected:
            break
        block.append(index)
        expected += 1
    return block


def collect_votes(ours_symbols, references):
    votes = defaultdict(list)
    boundaries = defaultdict(lambda: defaultdict(list))
    for name, start, end, family in LIBRARIES:
        ours = [symbol for symbol in ours_symbols if start <= symbol["addr"] < end]
        if not ours:
            continue
        bits = size_bits(ours)
        for reference, label, entries in references:
            ref = family_entries(entries, family)
            if not ref:
                continue
            mapping = align(ours, ref, bits, SEED_BITS)
            for i, (j, method, length, score) in mapping.items():
                votes[ours[i]["addr"]].append({"name": ref[j]["name"], "ref": label, "method": method, "run": length, "unit": ref[j]["unit"], "ref_addr": ref[j]["addr"], "library": name})
                if ref[j]["unit"] is not None:
                    boundaries[(name, label)][ref[j]["unit"]].append((ours[i]["addr"], j))
    return votes, boundaries


def derive_boundaries(boundaries, references, ours_by_addr):
    entries_by_label = {label: entries for reference, label, entries in references}
    files = []
    for (library, label), units in boundaries.items():
        entries = entries_by_label[label]
        for unit, hits in units.items():
            unit_indices = [j for j, entry in enumerate(entries) if entry["unit"] == unit and entry["section"] in CODE_SECTIONS]
            hits.sort()
            mapped = {j for addr, j in hits}
            first_addr = hits[0][0]
            last_addr = hits[-1][0]
            files.append({
                "library": library,
                "ref": label,
                "unit": unit,
                "first": first_addr,
                "last": last_addr,
                "last_end": last_addr + ours_by_addr[last_addr]["size"],
                "mapped": len(mapped),
                "functions": len(unit_indices),
                "starts_exact": bool(unit_indices) and unit_indices[0] in mapped,
                "ends_exact": bool(unit_indices) and unit_indices[-1] in mapped,
            })
    files.sort(key=lambda item: (item["first"], -item["mapped"]))
    return files


def game_of(label):
    return label[:-3] if label.startswith("wii-symbols-") else label


def base_name(name):
    split = name.find("__", 2)
    return name if split < 0 else name[:split]


def reference_rank(label):
    for rank, reference in enumerate(REFERENCES):
        if label.startswith(reference["label"]):
            return rank
    return len(REFERENCES)


def choose_name(candidates):
    names = defaultdict(set)
    for candidate in candidates:
        names[candidate["name"]].add(game_of(candidate["ref"]))
    if len({base_name(name) for name in names}) > 1:
        return None, False
    ranked = sorted(names, key=lambda name: (-len(names[name]), min(reference_rank(candidate["ref"]) for candidate in candidates if candidate["name"] == name)))
    return ranked[0], len(names) > 1


def confidence_of(candidates):
    games = {game_of(candidate["ref"]) for candidate in candidates}
    methods = {candidate["method"] for candidate in candidates}
    if "seed" in methods and len(games) > 1:
        return "high"
    if "seed" in methods or "unique" in methods or ("run" in methods and len(games) > 1):
        return "medium"
    return "low"


def consolidate_files(files):
    spans = {}
    for item in files:
        key = (item["library"], item["unit"].replace("\\", "/").rsplit("/", 1)[-1])
        span = spans.setdefault(key, {"library": key[0], "file": key[1], "first": item["first"], "end": item["last_end"], "refs": [], "starts_exact": False, "ends_exact": False, "mapped": 0})
        span["first"] = min(span["first"], item["first"])
        span["end"] = max(span["end"], item["last_end"])
        span["refs"].append(f"{item['ref']}:{item['unit']}")
        span["starts_exact"] = span["starts_exact"] or item["starts_exact"]
        span["ends_exact"] = span["ends_exact"] or item["ends_exact"]
        span["mapped"] = max(span["mapped"], item["mapped"])
    ordered = sorted(spans.values(), key=lambda span: (span["first"], span["end"]))
    for previous, current in zip(ordered, ordered[1:]):
        if current["first"] < previous["end"]:
            previous["overlaps"] = previous.get("overlaps", []) + [current["file"]]
            current["overlaps"] = current.get("overlaps", []) + [previous["file"]]
    return ordered


def decide(ours_symbols, votes, existing_names):
    by_addr = {symbol["addr"]: symbol for symbol in ours_symbols}
    accepted = {}
    conflicts = []
    agreement = Counter()
    for addr, candidates in votes.items():
        symbol = by_addr[addr]
        candidates = [candidate for candidate in candidates if not REF_PLACEHOLDER.search(candidate["name"])]
        if not candidates:
            continue
        name, signature_varies = choose_name(candidates)
        details = [[candidate["ref"], candidate["method"], candidate["run"], candidate["name"]] for candidate in candidates]
        if not PLACEHOLDER.match(symbol["name"]):
            if name is not None and base_name(name) == base_name(symbol["name"]):
                agreement["agree"] += 1
            else:
                agreement["disagree"] += 1
                conflicts.append({"addr": addr, "current": symbol["name"], "votes": details})
            continue
        if name is None:
            conflicts.append({"addr": addr, "current": symbol["name"], "votes": details})
            continue
        chosen = [candidate for candidate in candidates if base_name(candidate["name"]) == base_name(name)]
        accepted[addr] = {"name": name, "candidates": chosen, "signature_varies": signature_varies}
    counts = Counter(entry["name"] for entry in accepted.values())
    renames = []
    skipped = []
    for addr, entry in sorted(accepted.items()):
        if entry["name"] in existing_names or counts[entry["name"]] > 1:
            skipped.append({"addr": addr, "name": entry["name"], "reason": "name already used" if entry["name"] in existing_names else "name proposed for several symbols"})
            continue
        candidates = entry["candidates"]
        renames.append({
            "addr": addr,
            "old": by_addr[addr]["name"],
            "new": entry["name"],
            "section": by_addr[addr]["section"],
            "library": candidates[0]["library"],
            "sources": sorted({candidate["ref"] for candidate in candidates}),
            "games": len({game_of(candidate["ref"]) for candidate in candidates}),
            "methods": sorted({candidate["method"] for candidate in candidates}),
            "run": max(candidate["run"] for candidate in candidates),
            "confidence": confidence_of(candidates),
            "signature_varies": entry["signature_varies"],
            "units": sorted({candidate["unit"] for candidate in candidates if candidate["unit"]}),
        })
    return renames, conflicts, skipped, agreement


def apply_renames(path, renames):
    text = path.read_bytes().decode("utf-8")
    lines = text.split("\n")
    by_line = {}
    for symbol in parse_symbols(path):
        by_line[(symbol["section"], symbol["addr"], symbol["name"])] = symbol["line"]
    for rename in renames:
        number = by_line[(rename["section"], rename["addr"], rename["old"])]
        prefix = rename["old"] + " = "
        if not lines[number].startswith(prefix):
            raise ValueError(f"Unexpected line {number + 1}: {lines[number]}")
        lines[number] = rename["new"] + " = " + lines[number][len(prefix):]
    path.write_bytes("\n".join(lines).encode("utf-8"))


def data_votes(ours_all, references):
    votes = defaultdict(list)
    for section in DATA_SECTIONS:
        ours = [symbol for symbol in ours_all if symbol["section"] == section and symbol["type"] == "object" and symbol["size"] > 0]
        ours.sort(key=lambda symbol: symbol["addr"])
        if not ours:
            continue
        bits = size_bits(ours)
        for reference, label, entries in references:
            ref = [entry for entry in entries if entry["section"] == section and entry["unit"] is not None and any(FAMILY_PATTERNS[family].search(entry["unit"]) for family in ("sdk", "msl", "nw4r", "hbm", "revoex"))]
            if not ref:
                continue
            mapping = align(ours, ref, bits, SEED_BITS)
            for i, (j, method, length, score) in mapping.items():
                if method != "seed" and method != "run":
                    continue
                library = data_library(ref[j]["unit"])
                votes[ours[i]["addr"]].append({"name": ref[j]["name"], "ref": label, "method": method, "run": length, "unit": ref[j]["unit"], "ref_addr": ref[j]["addr"], "library": library})
    return votes


def data_library(unit):
    for family in ("hbm", "nw4r", "msl", "revoex", "sdk"):
        exclude = FAMILY_EXCLUDE.get(family)
        if FAMILY_PATTERNS[family].search(unit) and not (exclude and exclude.search(unit)):
            return f"{family}-data"
    return "data"


def fetch_command(args):
    destination = Path(args.dest)
    destination.mkdir(parents=True, exist_ok=True)
    for reference in REFERENCES:
        target = destination / reference["repo"].replace("/", "_")
        if not target.exists():
            subprocess.run(["git", "clone", "--filter=blob:none", "--no-checkout", f"https://github.com/{reference['repo']}", str(target)], check=True)
        subprocess.run(["git", "-C", str(target), "fetch", "--depth", "1", "origin", reference["commit"]], check=True)
        subprocess.run(["git", "-C", str(target), "sparse-checkout", "set", "--no-cone", reference["path"]], check=True)
        subprocess.run(["git", "-C", str(target), "checkout", "--detach", reference["commit"]], check=True)
        print(f"{reference['label']}: {reference['repo']} {reference['commit']} ({reference['license']})")
    return 0


def import_command(args):
    refs_dir = Path(args.refs)
    symbols_path = Path(args.symbols)
    ours_all = parse_symbols(symbols_path)
    existing_names = {symbol["name"] for symbol in ours_all}
    code = [symbol for symbol in ours_all if symbol["section"] in CODE_SECTIONS and symbol["type"] == "function" and symbol["size"] > 0]
    code.sort(key=lambda symbol: symbol["addr"])
    references = reference_sets(refs_dir, CODE_SECTIONS)
    if not references:
        raise ValueError(f"No references under {refs_dir}; run the fetch command first")
    votes, boundaries = collect_votes(code, references)
    renames, conflicts, skipped, agreement = decide(code, votes, existing_names)
    data_renames = []
    data_conflicts = []
    data_skipped = []
    data_agreement = Counter()
    if args.data:
        data_refs = reference_sets(refs_dir, DATA_SECTIONS)
        objects = [symbol for symbol in ours_all if symbol["section"] in DATA_SECTIONS]
        dvotes = data_votes(objects, data_refs)
        used = existing_names | {rename["new"] for rename in renames}
        data_renames, data_conflicts, data_skipped, data_agreement = decide(objects, dvotes, used)
        data_renames = [rename for rename in data_renames if rename["confidence"] != "low"]
    by_addr = {symbol["addr"]: symbol for symbol in code}
    files = derive_boundaries(boundaries, references, by_addr)
    report = {
        "references": REFERENCES,
        "seed_bits": SEED_BITS,
        "functions": {"renames": renames, "conflicts": conflicts, "skipped": skipped, "existing_agree": agreement["agree"], "existing_disagree": agreement["disagree"]},
        "data": {"renames": data_renames, "conflicts": data_conflicts, "skipped": data_skipped, "existing_agree": data_agreement["agree"], "existing_disagree": data_agreement["disagree"]},
        "file_spans": consolidate_files(files),
        "files": files,
    }
    Path(args.report).write_text(json.dumps(report, indent=1), encoding="utf-8")
    summary = Counter((rename["library"], rename["confidence"]) for rename in renames + data_renames)
    for (library, confidence), count in sorted(summary.items()):
        print(f"{library:24} {confidence:6} {count}")
    print(f"functions: {len(renames)} renames, {len(conflicts)} conflicts, {len(skipped)} skipped, existing names agree {agreement['agree']} disagree {agreement['disagree']}")
    if args.data:
        print(f"data: {len(data_renames)} renames, {len(data_conflicts)} conflicts, {len(data_skipped)} skipped, existing names agree {data_agreement['agree']} disagree {data_agreement['disagree']}")
    print(f"file boundaries: {len(files)} reference files, {len(report['file_spans'])} consolidated spans")
    chosen = [rename for rename in renames + data_renames if rename["confidence"] in args.confidence]
    if args.library:
        chosen = [rename for rename in chosen if any(rename["library"].startswith(prefix) for prefix in args.library)]
    if args.table:
        write_table(Path(args.table), chosen)
    if args.spans:
        write_spans(Path(args.spans), report["file_spans"])
    if args.apply:
        apply_renames(symbols_path, chosen)
        print(f"applied {len(chosen)} renames to {symbols_path}")
    return 0


def write_table(path, renames):
    header = ["address", "section", "library", "confidence", "method", "run", "games", "old", "name", "sources"]
    rows = ["\t".join(header)]
    for rename in sorted(renames, key=lambda item: (item["section"], item["addr"])):
        rows.append("\t".join([f"0x{rename['addr']:08X}", rename["section"], rename["library"], rename["confidence"], "+".join(rename["methods"]), str(rename["run"]), str(rename["games"]), rename["old"], rename["new"], ",".join(rename["sources"])]))
    path.write_text("\n".join(rows) + "\n", encoding="utf-8", newline="\n")


def write_spans(path, spans):
    header = ["first", "end", "library", "file", "mapped", "starts_exact", "ends_exact", "overlaps", "references"]
    rows = ["\t".join(header)]
    for span in spans:
        rows.append("\t".join([f"0x{span['first']:08X}", f"0x{span['end']:08X}", span["library"], span["file"], str(span["mapped"]), str(span["starts_exact"]).lower(), str(span["ends_exact"]).lower(), ",".join(span.get("overlaps", [])), ",".join(span["refs"])]))
    path.write_text("\n".join(rows) + "\n", encoding="utf-8", newline="\n")


def main():
    parser = argparse.ArgumentParser(description="Import library symbol names from reference symbol maps by aligning ordered function sizes.")
    commands = parser.add_subparsers(dest="command", required=True)
    fetch = commands.add_parser("fetch", help="Clone the pinned reference symbol maps")
    fetch.add_argument("--dest", required=True)
    fetch.set_defaults(action=fetch_command)
    run = commands.add_parser("import", help="Align references and report or apply renames")
    run.add_argument("--refs", required=True)
    run.add_argument("--symbols", default=str(SYMBOLS))
    run.add_argument("--report", required=True)
    run.add_argument("--data", action="store_true")
    run.add_argument("--apply", action="store_true")
    run.add_argument("--table")
    run.add_argument("--spans")
    run.add_argument("--confidence", nargs="+", default=["high", "medium"], choices=["high", "medium", "low"])
    run.add_argument("--library", action="append", default=[])
    run.set_defaults(action=import_command)
    args = parser.parse_args()
    try:
        return args.action(args)
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
