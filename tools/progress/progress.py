import argparse
import json
import sys
from pathlib import Path
from xml.sax.saxutils import escape


PHASE_STATES = ("done", "active", "pending")
WIDTH = 800
GUTTER = 16
GAP = 2
FONT = "-apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif"

THEME_LIGHT = {
    "surface": "#fcfcfb",
    "border": "#e4e3df",
    "text-primary": "#0b0b0b",
    "text-secondary": "#52514e",
    "fill": "#2a78d6",
    "active": "#86b6ef",
    "track": "#e8e7e3",
}
THEME_DARK = {
    "surface": "#1a1a19",
    "border": "#383835",
    "text-primary": "#f0efec",
    "text-secondary": "#c3c2b7",
    "fill": "#3987e5",
    "active": "#184f95",
    "track": "#2c2c2a",
}
RAMP_LIGHT = ["#cde2fb", "#9ec5f4", "#6da7ec", "#3987e5", "#256abf", "#184f95", "#0d366b"]
RAMP_DARK = ["#262625", "#0d366b", "#104281", "#1c5cab", "#2a78d6", "#5598e7", "#86b6ef"]
RAMP_LABELS = ["0%", "1-24%", "25-49%", "50-74%", "75-89%", "90-99%", "100%"]


def load_phases(path):
    document = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(document, dict) or document.get("schema_version") != 1:
        raise ValueError("Expected a phases object with schema_version 1")
    phases = document.get("phases")
    if not isinstance(phases, list) or not phases:
        raise ValueError("Phases file requires at least one phase")
    order = []
    for index, phase in enumerate(phases):
        if not isinstance(phase, dict) or phase.get("id") != index:
            raise ValueError(f"Phase ids must be consecutive from 0; problem at position {index}")
        if not isinstance(phase.get("title"), str) or not phase["title"].strip():
            raise ValueError(f"Phase {index} requires a nonempty title")
        if phase.get("state") not in PHASE_STATES:
            raise ValueError(f"Phase {index} state must be one of {', '.join(PHASE_STATES)}")
        order.append(PHASE_STATES.index(phase["state"]))
    if order != sorted(order):
        raise ValueError("Phase states must run done, then active, then pending")
    if order.count(PHASE_STATES.index("active")) > 1:
        raise ValueError("At most one phase may be active")
    return phases


def count(measures, key):
    value = measures.get(key, 0)
    if isinstance(value, str):
        value = int(value)
    if type(value) is not int or value < 0:
        raise ValueError(f"Invalid report measure {key}: {value!r}")
    return value


def percent(measures, key):
    value = measures.get(key, 0.0)
    if not isinstance(value, (int, float)) or not 0 <= value <= 100:
        raise ValueError(f"Invalid report percentage {key}: {value!r}")
    return float(value)


def load_report(path):
    report = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(report, dict) or not isinstance(report.get("measures"), dict) or not isinstance(report.get("units"), list):
        raise ValueError("Expected an objdiff report with measures and units")
    if count(report["measures"], "total_code") == 0:
        raise ValueError("Report has no code; the progress denominator would be zero")
    return report


def theme_style():
    def variables(theme):
        return " ".join(f"--{name}: {value};" for name, value in theme.items())

    def ramp(colors):
        return " ".join(f".b{index} {{ fill: {color}; }}" for index, color in enumerate(colors))

    return (
        "<style>"
        f"svg {{ {variables(THEME_LIGHT)} }} {ramp(RAMP_LIGHT)} "
        f"@media (prefers-color-scheme: dark) {{ svg {{ {variables(THEME_DARK)} }} {ramp(RAMP_DARK)} }} "
        f"text {{ font-family: {FONT}; fill: var(--text-primary); }} "
        ".secondary { fill: var(--text-secondary); } "
        ".title { font-size: 16px; font-weight: 600; } "
        ".label { font-size: 13px; } "
        ".small { font-size: 11px; } "
        ".surface { fill: var(--surface); stroke: var(--border); } "
        ".track { fill: var(--track); } "
        ".fill { fill: var(--fill); } "
        ".active { fill: var(--active); } "
        ".cell { stroke: var(--surface); stroke-width: 1; }"
        "</style>"
    )


def svg_document(height, title, body):
    return (
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" height="{height}" '
        f'viewBox="0 0 {WIDTH} {height}" role="img" aria-label="{escape(title, {chr(34): "&quot;"})}">'
        f"<title>{escape(title)}</title>{theme_style()}"
        f'<rect class="surface" x="0.5" y="0.5" width="{WIDTH - 1}" height="{height - 1}" rx="8"/>'
        f"{''.join(body)}</svg>\n"
    )


def text(x, y, content, css="label", anchor="start"):
    return f'<text x="{x:g}" y="{y:g}" class="{css}" text-anchor="{anchor}">{escape(content)}</text>'


def wrap(words, limit):
    lines = []
    for word in words.split():
        if lines and len(lines[-1]) + 1 + len(word) <= limit:
            lines[-1] = f"{lines[-1]} {word}"
        else:
            lines.append(word)
    return lines


def render_roadmap(phases):
    done = sum(phase["state"] == "done" for phase in phases)
    active = next((phase for phase in phases if phase["state"] == "active"), None)
    last = len(phases) - 1
    if active:
        status = f"Phase {active['id']} of {last} in progress: {active['title']}"
    elif done == len(phases):
        status = f"All {len(phases)} phases complete"
    else:
        status = f"{done} of {len(phases)} phases complete"
    count_phases = len(phases)
    segment = (WIDTH - 2 * GUTTER - GAP * (count_phases - 1)) / count_phases
    body = [text(GUTTER, 30, "Roadmap", "title"), text(GUTTER, 52, status, "label secondary")]
    bar_y = 70
    legend = {"done": "Done", "active": "In progress", "pending": "Not started"}
    for index, phase in enumerate(phases):
        x = GUTTER + index * (segment + GAP)
        css = {"done": "fill", "active": "active", "pending": "track"}[phase["state"]]
        body.append(
            f'<rect class="{css}" x="{x:.2f}" y="{bar_y}" width="{segment:.2f}" height="14" rx="4">'
            f"<title>Phase {phase['id']}: {escape(phase['title'])} ({legend[phase['state']]})</title></rect>"
        )
        body.append(text(x, bar_y + 34, f"{phase['id']} · {legend[phase['state']]}", "small secondary"))
        for line_index, line in enumerate(wrap(phase["title"], 15)):
            body.append(text(x, bar_y + 52 + line_index * 15, line, "small"))
    return svg_document(170, f"Roadmap: {status}", body)


def human_bytes(value):
    if value >= 1024 * 1024:
        return f"{value / (1024 * 1024):.2f} MiB"
    if value >= 1024:
        return f"{value / 1024:.1f} KiB"
    return f"{value} B"


def meter_row(y, label, matched, total, unit):
    share = matched / total * 100 if total else 0.0
    track_x = 220
    track_width = WIDTH - track_x - 230
    fill_width = track_width * share / 100
    amount = f"{human_bytes(matched)} of {human_bytes(total)}" if unit == "bytes" else f"{matched:,} of {total:,}"
    row = [
        text(GUTTER, y + 11, label),
        f'<rect class="track" x="{track_x}" y="{y}" width="{track_width}" height="14" rx="4"/>',
    ]
    if fill_width > 0:
        row.append(f'<rect class="fill" x="{track_x}" y="{y}" width="{max(fill_width, 4):.2f}" height="14" rx="4"/>')
    row.append(text(WIDTH - GUTTER, y + 11, f"{share:.2f}% · {amount}", "label secondary", "end"))
    return row


def render_code(report):
    if report is None:
        body = [
            text(GUTTER, 30, "Code progress", "title"),
            f'<rect class="track" x="{GUTTER}" y="48" width="{WIDTH - 2 * GUTTER}" height="14" rx="4"/>',
            text(GUTTER, 86, "Not measured. No objdiff report exists yet, so there is no progress denominator.", "label secondary"),
            text(GUTTER, 106, "Measurement begins at roadmap Phase 4.", "label secondary"),
        ]
        return svg_document(126, "Code progress: not measured", body)
    measures = report["measures"]
    rows = [
        ("Matched code", count(measures, "matched_code"), count(measures, "total_code"), "bytes"),
        ("Linked from source", count(measures, "complete_code"), count(measures, "total_code"), "bytes"),
        ("Matched functions", count(measures, "matched_functions"), count(measures, "total_functions"), "items"),
        ("Matched data", count(measures, "matched_data"), count(measures, "total_data"), "bytes"),
    ]
    categories = [category for category in report.get("categories", []) if count(category.get("measures", {}), "total_code")]
    if sum(count(category["measures"], "total_code") for category in categories) == count(measures, "total_code"):
        for category in categories:
            category_measures = category["measures"]
            rows.append((f"{category.get('name', category.get('id', '?'))} code", count(category_measures, "matched_code"), count(category_measures, "total_code"), "bytes"))
    body = [text(GUTTER, 30, "Code progress", "title")]
    y = 48
    for label, matched, total, unit in rows:
        body.extend(meter_row(y, label, matched, total, unit))
        y += 26
    body.append(text(GUTTER, y + 10, "Matched: source compiles to identical code. Linked: the final build uses that source instead of an original fallback object.", "small secondary"))
    headline = rows[0][1] / rows[0][2] * 100
    return svg_document(y + 26, f"Code progress: {headline:.2f}% matched code", body)


def worst_ratio(row, side):
    total = sum(row)
    return max(max(row) * side * side / (total * total), total * total / (side * side * min(row)))


def squarify(values, x, y, width, height):
    rectangles = []
    index = 0
    while index < len(values):
        side = min(width, height)
        row = [values[index]]
        index += 1
        while index < len(values) and worst_ratio(row + [values[index]], side) <= worst_ratio(row, side):
            row.append(values[index])
            index += 1
        total = sum(row)
        if width >= height:
            column = total / height if height else 0
            offset = y
            for value in row:
                cell = value / column if column else 0
                rectangles.append((x, offset, column, cell))
                offset += cell
            x += column
            width -= column
        else:
            band = total / width if width else 0
            offset = x
            for value in row:
                cell = value / band if band else 0
                rectangles.append((offset, y, cell, band))
                offset += cell
            y += band
            height -= band
    return rectangles


def ramp_bin(match):
    if match >= 100:
        return 6
    if match <= 0:
        return 0
    for index, ceiling in enumerate((25, 50, 75, 90, 100), start=1):
        if match < ceiling:
            return index
    return 5


def render_treemap(report):
    units = []
    for unit in report["units"]:
        measures = unit.get("measures", {})
        size = count(measures, "total_code")
        if size:
            units.append((size, unit.get("name", "?"), percent(measures, "fuzzy_match_percent")))
    if not units:
        raise ValueError("Report has no units with code")
    units.sort(key=lambda item: (-item[0], item[1]))
    map_x, map_y = GUTTER, 48
    map_width, map_height = WIDTH - 2 * GUTTER, 400
    area = map_width * map_height
    total = sum(size for size, _, _ in units)
    scaled = [size * area / total for size, _, _ in units]
    body = [text(GUTTER, 30, "Code by translation unit", "title"),
            text(WIDTH - GUTTER, 30, "Area: code size · Shade: match", "label secondary", "end")]
    for (size, name, match), (x, y, width, height) in zip(units, squarify(scaled, map_x, map_y, map_width, map_height)):
        body.append(
            f'<rect class="cell b{ramp_bin(match)}" x="{x:.2f}" y="{y:.2f}" width="{width:.2f}" height="{height:.2f}">'
            f"<title>{escape(name)}: {match:.2f}% matched, {human_bytes(size)}</title></rect>"
        )
    legend_y = map_y + map_height + 20
    swatch = (WIDTH - 2 * GUTTER - GAP * (len(RAMP_LABELS) - 1)) / len(RAMP_LABELS)
    for index, label in enumerate(RAMP_LABELS):
        x = GUTTER + index * (swatch + GAP)
        body.append(f'<rect class="b{index}" x="{x:.2f}" y="{legend_y}" width="{swatch:.2f}" height="10" rx="2"/>')
        body.append(text(x, legend_y + 26, label, "small secondary"))
    return svg_document(legend_y + 44, "Code by translation unit, shaded by match percentage", body)


def write_output(path, content):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")
    print(f"Wrote {path}")


def render(args):
    if args.treemap and not args.report:
        raise ValueError("--treemap requires --report")
    phases = load_phases(args.phases)
    report = load_report(args.report) if args.report else None
    output = args.output_dir
    write_output(output / "progress-roadmap.svg", render_roadmap(phases))
    write_output(output / "progress-code.svg", render_code(report))
    if args.treemap:
        write_output(output / "progress-treemap.svg", render_treemap(report))
    return 0


def main():
    parser = argparse.ArgumentParser(description="Render roadmap and code progress images.")
    commands = parser.add_subparsers(dest="command", required=True)
    draw = commands.add_parser("render", help="Write progress SVG images")
    draw.add_argument("--phases", type=Path, required=True)
    draw.add_argument("--report", type=Path, help="objdiff progress report JSON; omit until a denominator exists")
    draw.add_argument("--output-dir", type=Path, required=True)
    draw.add_argument("--treemap", action="store_true", help="also write the translation-unit treemap")
    draw.set_defaults(action=render)
    args = parser.parse_args()
    try:
        return args.action(args)
    except (OSError, ValueError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
