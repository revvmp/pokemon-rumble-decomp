import argparse
import json
import re
import shutil
from pathlib import Path


def prepare(specification, template, output_dir):
    inputs = json.loads(specification.read_text(encoding="utf-8"))
    selectors = []
    aliases = []
    output_dir.mkdir(parents=True, exist_ok=True)
    for index, entry in enumerate(inputs):
        source = Path(entry["object"])
        sections = entry.get("text_sections", [".text"])
        if ".text" not in sections or len(sections) != len(set(sections)):
            raise ValueError(f"Invalid text section order for {source}: {sections}")
        if any(re.fullmatch(r"\.text(?:[.$][A-Za-z0-9_]+)*", section) is None for section in sections):
            raise ValueError(f"Invalid text section name for {source}: {sections}")
        alias = output_dir / f"unit_{index:04d}.o"
        if source.resolve() == alias.resolve():
            raise ValueError(f"Link alias overlaps its source: {source}")
        shutil.copyfile(source, alias)
        aliases.append(alias)
        selectors.extend(f'            "{alias.name}"({section})' for section in sections)
    original = template.read_text(encoding="utf-8")
    layout, count = re.subn(
        r"(?m)^(\s*\.text\s+ALIGN\([^)]*\):)\s*\{\s*\}",
        lambda match: match[1] + "{\n" + "\n".join(selectors) + "\n        }",
        original,
    )
    if count != 1:
        raise ValueError(f"Expected one empty .text output section in {template}, found {count}")
    (output_dir / "ordered.lcf").write_text(layout, encoding="utf-8", newline="\n")
    response = "\n".join(f'"{alias.as_posix()}"' for alias in aliases) + "\n"
    (output_dir / "inputs.rsp").write_text(response, encoding="utf-8", newline="\n")


def main():
    parser = argparse.ArgumentParser(description="Prepare byte-identical linker aliases and ordered native .text selectors.")
    parser.add_argument("--specification", type=Path, required=True)
    parser.add_argument("--template", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    prepare(args.specification, args.template, args.output_dir)


if __name__ == "__main__":
    main()
