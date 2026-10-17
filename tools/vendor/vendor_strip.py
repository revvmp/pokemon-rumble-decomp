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


def main():
    parser = argparse.ArgumentParser(description="Copy Python sources without comments or docstrings.")
    parser.add_argument("sources", type=Path, nargs="+")
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    try:
        for source_path in args.sources:
            original = source_path.read_text(encoding="utf-8")
            stripped = strip_source(original)
            verify_equivalent(original, stripped)
            target = args.output_dir / source_path.name
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(stripped, encoding="utf-8", newline="\n")
            print(f"Wrote {target}")
    except (OSError, SyntaxError, ValueError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
