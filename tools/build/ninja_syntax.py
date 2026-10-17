import re
import textwrap
import os
from io import StringIO
from pathlib import Path
from typing import Dict, Iterable, List, Match, Optional, Tuple, Union

NinjaPath = Union[str, Path]
NinjaPaths = Iterable[Optional[NinjaPath]]
NinjaPathOrPaths = Union[NinjaPath, NinjaPaths]


def escape_path(word: str) -> str:
    return word.replace("$ ", "$$ ").replace(" ", "$ ").replace(":", "$:")


class Writer(object):
    def __init__(self, output: StringIO, width: int = 78) -> None:
        self.output = output
        self.width = width

    def newline(self) -> None:
        self.output.write("\n")

    def comment(self, text: str) -> None:
        for line in textwrap.wrap(
            text, self.width - 2, break_long_words=False, break_on_hyphens=False
        ):
            self.output.write("# " + line + "\n")

    def variable(
        self,
        key: str,
        value: Optional[NinjaPathOrPaths],
        indent: int = 0,
    ) -> None:
        value = " ".join(serialize_paths(value))
        self._line("%s = %s" % (key, value), indent)

    def pool(self, name: str, depth: int) -> None:
        self._line("pool %s" % name)
        self.variable("depth", str(depth), indent=1)

    def rule(
        self,
        name: str,
        command: str,
        description: Optional[str] = None,
        depfile: Optional[NinjaPath] = None,
        generator: bool = False,
        pool: Optional[str] = None,
        restat: bool = False,
        rspfile: Optional[NinjaPath] = None,
        rspfile_content: Optional[NinjaPath] = None,
        deps: Optional[NinjaPathOrPaths] = None,
    ) -> None:
        self._line("rule %s" % name)
        self.variable("command", command, indent=1)
        if description:
            self.variable("description", description, indent=1)
        if depfile:
            self.variable("depfile", depfile, indent=1)
        if generator:
            self.variable("generator", "1", indent=1)
        if pool:
            self.variable("pool", pool, indent=1)
        if restat:
            self.variable("restat", "1", indent=1)
        if rspfile:
            self.variable("rspfile", rspfile, indent=1)
        if rspfile_content:
            self.variable("rspfile_content", rspfile_content, indent=1)
        if deps:
            self.variable("deps", deps, indent=1)

    def build(
        self,
        outputs: NinjaPathOrPaths,
        rule: str,
        inputs: Optional[NinjaPathOrPaths] = None,
        implicit: Optional[NinjaPathOrPaths] = None,
        order_only: Optional[NinjaPathOrPaths] = None,
        variables: Optional[
            Union[
                List[Tuple[str, Optional[NinjaPathOrPaths]]],
                Dict[str, Optional[NinjaPathOrPaths]],
            ]
        ] = None,
        implicit_outputs: Optional[NinjaPathOrPaths] = None,
        pool: Optional[str] = None,
        dyndep: Optional[NinjaPath] = None,
    ) -> List[str]:
        str_outputs = serialize_paths(outputs)
        out_outputs = [escape_path(x) for x in str_outputs]
        all_inputs = [escape_path(x) for x in serialize_paths(inputs)]

        if implicit:
            implicit = [escape_path(x) for x in serialize_paths(implicit)]
            all_inputs.append("|")
            all_inputs.extend(map(str, implicit))
        if order_only:
            order_only = [escape_path(x) for x in serialize_paths(order_only)]
            all_inputs.append("||")
            all_inputs.extend(map(str, order_only))
        if implicit_outputs:
            implicit_outputs = [
                escape_path(x) for x in serialize_paths(implicit_outputs)
            ]
            out_outputs.append("|")
            out_outputs.extend(map(str, implicit_outputs))

        self._line(
            "build %s: %s" % (" ".join(out_outputs), " ".join([rule] + all_inputs))
        )
        if pool is not None:
            self._line("  pool = %s" % pool)
        if dyndep is not None:
            self._line("  dyndep = %s" % serialize_path(dyndep))

        if variables:
            if isinstance(variables, dict):
                iterator = iter(variables.items())
            else:
                iterator = iter(variables)

            for key, val in iterator:
                self.variable(key, val, indent=1)

        return str_outputs

    def include(self, path: str) -> None:
        self._line("include %s" % path)

    def subninja(self, path: str) -> None:
        self._line("subninja %s" % path)

    def default(self, paths: NinjaPathOrPaths) -> None:
        self._line("default %s" % " ".join(serialize_paths(paths)))

    def _count_dollars_before_index(self, s: str, i: int) -> int:
        dollar_count = 0
        dollar_index = i - 1
        while dollar_index > 0 and s[dollar_index] == "$":
            dollar_count += 1
            dollar_index -= 1
        return dollar_count

    def _line(self, text: str, indent: int = 0) -> None:
        leading_space = "  " * indent
        while len(leading_space) + len(text) > self.width:

            available_space = self.width - len(leading_space) - len(" $")
            space = available_space
            while True:
                space = text.rfind(" ", 0, space)
                if space < 0 or self._count_dollars_before_index(text, space) % 2 == 0:
                    break

            if space < 0:
                space = available_space - 1
                while True:
                    space = text.find(" ", space + 1)
                    if (
                        space < 0
                        or self._count_dollars_before_index(text, space) % 2 == 0
                    ):
                        break
            if space < 0:
                break

            self.output.write(leading_space + text[0:space] + " $\n")
            text = text[space + 1 :]

            leading_space = "  " * (indent + 2)

        self.output.write(leading_space + text + "\n")

    def close(self) -> None:
        self.output.close()


def serialize_path(input: Optional[NinjaPath]) -> str:
    if not input:
        return ""
    if isinstance(input, Path):
        return str(input).replace("/", os.sep)
    else:
        return str(input)


def serialize_paths(input: Optional[NinjaPathOrPaths]) -> List[str]:
    if isinstance(input, str) or isinstance(input, Path):
        return [serialize_path(input)] if input else []
    elif input is not None:
        return [serialize_path(path) for path in input if path]
    return []


def escape(string: str) -> str:
    assert "\n" not in string, "Ninja syntax does not allow newlines"
    return string.replace("$", "$$")


def expand(string: str, vars: Dict[str, str], local_vars: Dict[str, str] = {}) -> str:

    def exp(m: Match[str]) -> str:
        var = m.group(1)
        if var == "$":
            return "$"
        return local_vars.get(var, vars.get(var, ""))

    return re.sub(r"\$(\$|\w*)", exp, string)
