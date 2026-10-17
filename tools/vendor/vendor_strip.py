import argparse
import ast
import io
import sys
import tokenize
from pathlib import Path


def docstring_lines(source):
    lines = set()
    for node in ast.walk(ast.parse(source)):
        if isinstance(node, (ast.Module, ast.ClassDef, ast.FunctionDef, ast.AsyncFunctionDef)) and node.body:
            first = node.body[0]
            if isinstance(first, ast.Expr) and isinstance(first.value, ast.Constant) and isinstance(first.value.value, str):
                replacement = "pass" if len(node.body) == 1 else None
                lines.add((first.lineno, first.end_lineno, first.col_offset, replacement))
    return lines


def strip_source(source):
    lines = source.splitlines(keepends=True)
    for start, end, column, replacement in sorted(docstring_lines(source), reverse=True):
        indent = " " * column
        lines[start - 1:end] = [f"{indent}{replacement}\n"] if replacement else []
    source = "".join(lines)
    lines = source.splitlines(keepends=True)
    for token in reversed(list(tokenize.generate_tokens(io.StringIO(source).readline))):
        if token.type == tokenize.COMMENT:
            row, column = token.start
            line = lines[row - 1]
            kept = line[:column].rstrip()
            lines[row - 1] = f"{kept}\n" if kept else ""
    stripped = "".join(lines)
    while "\n\n\n\n" in stripped:
        stripped = stripped.replace("\n\n\n\n", "\n\n\n")
    ast.parse(stripped)
    return stripped.lstrip("\n")


def verify_equivalent(original, stripped):
    def normalized(source):
        tree = ast.parse(source)
        for node in ast.walk(tree):
            if isinstance(node, (ast.Module, ast.ClassDef, ast.FunctionDef, ast.AsyncFunctionDef)) and node.body:
                first = node.body[0]
                if isinstance(first, ast.Expr) and isinstance(first.value, ast.Constant) and isinstance(first.value.value, str):
                    node.body = node.body[1:] or [ast.Pass()]
        return ast.dump(tree, include_attributes=False)

    if normalized(original) != normalized(stripped):
        raise RuntimeError("Stripped source is not equivalent to the original")


C_SUFFIXES = {".c", ".h", ".cpp", ".hpp", ".cc", ".inc"}


def strip_c_source(source):
    output = []
    removed_lines = set()
    line = 0
    index = 0
    length = len(source)
    while index < length:
        char = source[index]
        pair = source[index:index + 2]
        if char in "\"'":
            end = index + 1
            while end < length and source[end] != char:
                if source[end] == "\\":
                    end += 1
                elif source[end] == "\n":
                    raise ValueError(f"Unterminated literal on line {line + 1}")
                end += 1
            output.append(source[index:end + 1])
            index = end + 1
        elif pair == "//":
            end = source.find("\n", index)
            end = length if end < 0 else end
            if source[index:end].rstrip().endswith("\\"):
                raise ValueError(f"Line comment continues onto the next line at line {line + 1}")
            removed_lines.add(line)
            index = end
        elif pair == "/*":
            end = source.find("*/", index + 2)
            if end < 0:
                raise ValueError(f"Unterminated block comment on line {line + 1}")
            newlines = source.count("\n", index, end + 2)
            removed_lines.update(range(line, line + newlines + 1))
            line += newlines
            output.append("\n" * newlines if newlines else " ")
            index = end + 2
        else:
            if char == "\n":
                line += 1
            output.append(char)
            index += 1
    original_lines = source.split("\n")
    kept = []
    for number, text in enumerate("".join(output).split("\n")):
        text = text.rstrip()
        if not text and number in removed_lines and original_lines[number].strip():
            continue
        kept.append(text)
    stripped = "\n".join(kept)
    while "\n\n\n" in stripped:
        stripped = stripped.replace("\n\n\n", "\n\n")
    return stripped.strip("\n") + "\n"


def verify_c_equivalent(original, stripped):
    if strip_c_source(stripped) != stripped:
        raise RuntimeError("Stripped C source still contains comments")
    if "".join(strip_c_source(original).split()) != "".join(stripped.split()):
        raise RuntimeError("Stripped C source is not equivalent to the original")


def main():
    parser = argparse.ArgumentParser(description="Copy Python or C/C++ sources without comments or docstrings.")
    parser.add_argument("sources", type=Path, nargs="+")
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--root", type=Path, help="keep each source's path relative to this directory under the output directory")
    parser.add_argument("--skip-existing", action="store_true", help="leave files that already exist in the output directory untouched")
    args = parser.parse_args()
    try:
        for source_path in args.sources:
            relative = source_path.relative_to(args.root) if args.root else Path(source_path.name)
            target = args.output_dir / relative
            if args.skip_existing and target.exists():
                print(f"Kept existing {target}")
                continue
            original = source_path.read_text(encoding="utf-8")
            if source_path.suffix.lower() in C_SUFFIXES:
                stripped = strip_c_source(original)
                verify_c_equivalent(original, stripped)
            else:
                stripped = strip_source(original)
                verify_equivalent(original, stripped)
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(stripped, encoding="utf-8", newline="\n")
            print(f"Wrote {target}")
    except (OSError, SyntaxError, ValueError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
