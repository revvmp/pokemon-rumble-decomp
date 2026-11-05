import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile


DIRECTIVE = re.compile(r'^\s*(?:[A-Za-z_][A-Za-z_0-9]*:\s*)?(INCLUDE|INCDIR)\b(.*)$', re.IGNORECASE)
LITERAL = re.compile(r'^\s*"([^"\r\n]*)"\s*$')


class BuildError(Exception):
    pass


def positive_size(value):
    try:
        size = int(value, 0)
    except ValueError as error:
        raise argparse.ArgumentTypeError("size must be an integer byte count") from error
    if size <= 0 or size % 2:
        raise argparse.ArgumentTypeError("size must be a positive even byte count")
    return size


def file_path(value):
    path = Path(value).expanduser().resolve()
    if not path.is_file():
        raise BuildError(f"Input is not a regular file: {path}")
    return path


def output_path(value):
    requested = Path(value).expanduser()
    if requested.is_symlink():
        raise BuildError(f"Output must not be a symbolic link: {requested}")
    path = requested.resolve()
    if path.exists() and not path.is_file():
        raise BuildError(f"Output is not a regular file: {path}")
    parent = path.parent
    while not parent.exists():
        parent = parent.parent
    if not parent.is_dir():
        raise BuildError(f"Output parent is not a directory: {parent}")
    return path


def same_file(left, right):
    return left == right or (left.exists() and right.exists() and left.samefile(right))


def validate_collisions(inputs, outputs):
    for index, output in enumerate(outputs):
        for protected in inputs:
            if same_file(output, protected):
                raise BuildError(f"Output overlaps an input: {output} -> {protected}")
        for other in outputs[:index]:
            if same_file(output, other):
                raise BuildError(f"Outputs overlap: {output} -> {other}")


def source_lines(text, path):
    blocked = False
    for number, original in enumerate(text.splitlines(), 1):
        if len(original.encode("utf-8")) >= 1024:
            raise BuildError(f"Source line exceeds assembler limit: {path}:{number}")
        line = []
        index = 0
        while index < len(original):
            pair = original[index:index + 2]
            character = original[index]
            if pair == "//" or (character == ";" and not blocked):
                break
            if pair == "/*":
                blocked = not blocked
                line.extend("  ")
                index += 2
                continue
            if pair == "*/" and blocked:
                blocked = False
                line.extend("  ")
                index += 2
                continue
            line.append(" " if blocked or character == "\t" else character)
            index += 1
        yield number, "".join(line)


def dependencies(source):
    base = source.parent
    files = {}
    active = []

    def visit(path, include_directory):
        path = path.resolve()
        if path in active:
            chain = " -> ".join(str(item) for item in [*active, path])
            raise BuildError(f"Recursive assembly include: {chain}")
        if not path.is_file():
            raise BuildError(f"Assembly include does not exist: {path}")
        data = path.read_bytes()
        if path in files and files[path] != data:
            raise BuildError(f"Assembly input changed during dependency scan: {path}")
        files[path] = data
        try:
            text = data.decode("utf-8")
        except UnicodeDecodeError as error:
            raise BuildError(f"Assembly source is not UTF-8: {path}") from error
        active.append(path)
        for number, line in source_lines(text, path):
            match = DIRECTIVE.match(line)
            if match is None:
                continue
            literal = LITERAL.fullmatch(match[2])
            if literal is None or any(token in literal[1] for token in (":", ",", "\x00")):
                raise BuildError(f"Expected a literal {match[1].upper()} path without colon or comma: {path}:{number}")
            value = literal[1]
            if match[1].upper() == "INCDIR":
                include_directory = value
            else:
                if not value:
                    raise BuildError(f"Empty assembly include: {path}:{number}")
                name = include_directory + "/" + value if include_directory else value
                include_directory = visit(base / name, include_directory)
        active.pop()
        return include_directory

    include_directory = visit(source, "")
    visit(source, include_directory)
    return files


def depfile_path(path):
    value = path.as_posix()
    if any(character in value for character in ("\n", "\r", "\x00")):
        raise BuildError(f"Path cannot be represented in a depfile: {path}")
    return value.replace("$", "$$").replace("#", "\\#").replace(" ", "\\ ").replace("\t", "\\\t").replace(":", "\\:")


def create_temporary(destination):
    destination.parent.mkdir(parents=True, exist_ok=True)
    descriptor, name = tempfile.mkstemp(prefix=f".{destination.name}.", suffix=".tmp", dir=destination.parent)
    os.close(descriptor)
    return Path(name)


def write_temporary(destination, data):
    temporary = create_temporary(destination)
    try:
        with temporary.open("wb") as stream:
            stream.write(data)
            stream.flush()
            os.fsync(stream.fileno())
    except BaseException:
        temporary.unlink(missing_ok=True)
        raise
    return temporary


def publish(staged):
    backups = {}
    replaced = []
    retained_backups = set()
    try:
        for destination, temporary in staged.items():
            if destination.exists() and destination.read_bytes() == temporary.read_bytes():
                continue
            if destination.exists():
                backup = create_temporary(destination)
                backups[destination] = backup
                shutil.copyfile(destination, backup)
        for destination, temporary in staged.items():
            if destination.exists() and destination.read_bytes() == temporary.read_bytes():
                continue
            os.replace(temporary, destination)
            replaced.append(destination)
    except BaseException as publish_error:
        recovery_errors = []
        for destination in reversed(replaced):
            backup = backups.get(destination)
            try:
                if backup is None:
                    destination.unlink(missing_ok=True)
                else:
                    os.replace(backup, destination)
            except BaseException as recovery_error:
                detail = f"{destination}: {recovery_error}"
                if backup is not None:
                    retained_backups.add(backup)
                    detail += f"; original retained at {backup}"
                recovery_errors.append(detail)
        if recovery_errors:
            raise BuildError("Publishing DSP outputs failed; incomplete recovery: " + " | ".join(recovery_errors)) from publish_error
        raise
    finally:
        for temporary in [*staged.values(), *backups.values()]:
            if temporary not in retained_backups:
                temporary.unlink(missing_ok=True)


def build(args):
    assembler = file_path(args.assembler)
    source = file_path(args.source)
    outputs = [output_path(value) for value in (args.binary, args.c_include, args.depfile)]
    source_files = dependencies(source)
    inputs = [*source_files, assembler, Path(__file__).resolve()]
    validate_collisions(inputs, outputs)
    binary, c_include, depfile = outputs
    targets = " ".join(depfile_path(Path(value).expanduser()) for value in (args.binary, args.c_include))
    prerequisites = " ".join(depfile_path(path) for path in sorted(set(inputs)))
    dependency_text = f"{targets}: {prerequisites}\n".encode("utf-8")
    staged = {}
    try:
        staged[binary] = create_temporary(binary)
        result = subprocess.run([str(assembler), "assemble", str(source), str(staged[binary])], cwd=source.parent)
        if result.returncode:
            raise BuildError(f"DSP assembler failed with exit code {result.returncode}")
        data = staged[binary].read_bytes()
        if len(data) != args.size:
            raise BuildError(f"DSP image size is {len(data)} bytes; expected {args.size}")
        for path, expected in source_files.items():
            if path.read_bytes() != expected:
                raise BuildError(f"Assembly input changed during build: {path}")
        element_bytes = args.element_width // 8
        values = [int.from_bytes(data[offset:offset + element_bytes], "big") for offset in range(0, len(data), element_bytes)]
        values_per_line = 16 // element_bytes
        digits = element_bytes * 2
        initializer = "".join("    " + ", ".join(f"0x{value:0{digits}X}" for value in values[offset:offset + values_per_line]) + ",\n" for offset in range(0, len(values), values_per_line))
        staged[c_include] = write_temporary(c_include, initializer.encode("ascii"))
        staged[depfile] = write_temporary(depfile, dependency_text)
        validate_collisions(inputs, outputs)
        publish(staged)
    finally:
        for temporary in staged.values():
            temporary.unlink(missing_ok=True)
    print(f"Assembled {len(data)} DSP bytes from {len(source_files)} source files")


def main():
    parser = argparse.ArgumentParser(description="Assemble a DSP program and emit a C initializer and dependency file")
    parser.add_argument("--assembler", required=True)
    parser.add_argument("--source", required=True)
    parser.add_argument("--binary", required=True)
    parser.add_argument("--c-include", required=True)
    parser.add_argument("--depfile", required=True)
    parser.add_argument("--size", type=positive_size, default=8192)
    parser.add_argument("--element-width", type=int, choices=(8, 16), default=16, help="C initializer element width; default: big-endian 16-bit DSP words")
    args = parser.parse_args()
    try:
        build(args)
    except (BuildError, OSError) as error:
        print(f"DSP build failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
