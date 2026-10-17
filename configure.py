import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.build.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

VERSIONS = ["wpse-v1"]
LOCAL_TOOLS = Path("tools") / "local"


def local_tool(name):
    path = LOCAL_TOOLS / name
    return path if path.exists() else None


parser = argparse.ArgumentParser(description="Generate build.ninja and objdiff.json for a target.")
parser.add_argument("mode", choices=["configure", "progress"], default="configure", nargs="?")
parser.add_argument("-v", "--version", choices=VERSIONS, default=VERSIONS[0])
parser.add_argument("--build-dir", metavar="DIR", type=Path, default=Path("build"))
parser.add_argument("--binutils", metavar="DIR", type=Path, default=local_tool("binutils"))
parser.add_argument("--compilers", metavar="DIR", type=Path, default=local_tool("compilers"))
parser.add_argument("--dtk", metavar="BINARY", type=Path, default=local_tool("dtk.exe"))
parser.add_argument("--objdiff", metavar="BINARY", type=Path, default=local_tool("objdiff-cli.exe"))
parser.add_argument("--sjiswrap", metavar="EXE", type=Path, default=local_tool("sjiswrap.exe"))
parser.add_argument("--ninja", metavar="BINARY", type=Path)
parser.add_argument("--map", action="store_true")
parser.add_argument("--debug", action="store_true")
parser.add_argument("--verbose", action="store_true")
parser.add_argument("--non-matching", dest="non_matching", action="store_true")
parser.add_argument("--no-progress", dest="progress", action="store_false")
if not is_windows():
    parser.add_argument("--wrapper", metavar="BINARY", type=Path)
args = parser.parse_args()

config = ProjectConfig()
config.tools_dir = Path("tools") / "build"
config.version = args.version
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.generate_map = args.map
config.non_matching = args.non_matching
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
if not config.non_matching:
    config.asm_dir = None

config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.4"
config.objdiff_tag = "v3.8.2"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
]
config.ldflags = ["-fp hardware", "-nodefaults"]
if args.debug:
    config.ldflags.append("-gdwarf-2")
if args.map:
    config.ldflags.append("-mapunused")
    config.ldflags.append("-listclosure")
config.reconfig_deps = []
config.scratch_preset_id = None
config.linker_version = "Wii/1.1"

cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-enc SJIS",
    "-i include",
    f"-i build/{config.version}/include",
]
if args.debug:
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

Matching = True
NonMatching = False
Equivalent = config.non_matching


def library(name: str, mw_version: str, cflags: List[str], category: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": name,
        "mw_version": mw_version,
        "cflags": cflags,
        "progress_category": category,
        "objects": objects,
    }


config.warn_missing_config = False
config.warn_missing_source = False
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
    "-func_align 4",
]

RUNTIME_COMPILER = "Wii/1.1"

cflags_sdk = [
    *(flag for flag in cflags_base if flag not in ("-fp_contract on", "-inline auto")),
    "-inline auto,level=3",
    "-ipa file",
    "-sdata 8",
    "-sdata2 8",
]

SDK_COMPILER = "Wii/1.0"

NDEV_COMPILER = "GC/3.0a3"

config.libs = [
    library(
        "RVL_SDK.CNT",
        "Wii/1.0",
        cflags_base,
        "sdk",
        [Object(NonMatching, "RVL_SDK/cnt/cnt.c")],
    ),
    library(
        "RVL_SDK",
        SDK_COMPILER,
        cflags_sdk,
        "sdk",
        [
            Object(Matching, "RVL_SDK/base/PPCArch.c"),
            Object(NonMatching, "RVL_SDK/os/OSAlarm.c"),
            Object(NonMatching, "RVL_SDK/os/OSContext.c"),
            Object(Matching, "RVL_SDK/os/OSIpc.c"),
            Object(Matching, "RVL_SDK/exi/EXIUart.c"),
            Object(Matching, "RVL_SDK/exi/EXICommon.c"),
            Object(Matching, "RVL_SDK/si/SIBios.c"),
            Object(Matching, "RVL_SDK/si/SISamplingRate.c"),
            Object(Matching, "RVL_SDK/db/db.c"),
            Object(NonMatching, "RVL_SDK/vi/i2c.c"),
            Object(NonMatching, "RVL_SDK/gx/GXFifo.c"),
            Object(NonMatching, "RVL_SDK/gx/GXMisc.c"),
            Object(NonMatching, "RVL_SDK/gx/GXGeometry.c"),
            Object(NonMatching, "RVL_SDK/gx/GXFrameBuf.c"),
            Object(NonMatching, "RVL_SDK/gx/GXLight.c"),
            Object(Matching, "RVL_SDK/gx/GXTev.c"),
            Object(NonMatching, "RVL_SDK/gx/GXPixel.c"),
            Object(NonMatching, "RVL_SDK/gx/GXDisplayList.c"),
            Object(NonMatching, "RVL_SDK/ax/AX.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbStdExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXChorusExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXLfoTable.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXSrcCoef.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXHooks.c"),
            Object(Matching, "RVL_SDK/mem/mem_heapCommon.c"),
            Object(Matching, "RVL_SDK/mem/mem_expHeap.c"),
            Object(NonMatching, "RVL_SDK/nand/nand.c"),
            Object(Matching, "RVL_SDK/enc/enclatin.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/arc/arc.c"),
            Object(NonMatching, "RVL_SDK/esp/esp.c"),
            Object(NonMatching, "RVL_SDK/ipc/ipcMain.c"),
            Object(Matching, "RVL_SDK/ipc/memory.c"),
            Object(Matching, "RVL_SDK/ipc/ipcProfile.c"),
            Object(NonMatching, "RVL_SDK/bte/bte_hcisu.c", cflags=cflags_base),
            Object(NonMatching, "RVL_SDK/bte/bte_init.c", cflags=cflags_base),
            Object(NonMatching, "RVL_SDK/bte/bte_logmsg.c", cflags=cflags_base),
            Object(NonMatching, "RVL_SDK/bte/bte_main.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/bta_sys_conn.c", cflags=[*cflags_base, "-func_align 4"]),
            Object(NonMatching, "RVL_SDK/bte/bta_dm_api.c", cflags=[*cflags_base, "-func_align 4"]),
            Object(Matching, "RVL_SDK/gd/GDTexture.c"),
        ],
    ),
    library(
        "NdevExi2A",
        NDEV_COMPILER,
        cflags_sdk,
        "sdk",
        [
            Object(Matching, "RVL_SDK/NdevExi2A/DebuggerDriver.c"),
            Object(Matching, "RVL_SDK/NdevExi2A/exi2.c"),
        ],
    ),
    library(
        "Runtime.PPCEABI.H",
        RUNTIME_COMPILER,
        cflags_runtime,
        "runtime",
        [Object(Matching, "Runtime/__init_cpp_exceptions.cpp")],
    ),
]

config.progress_categories = [
    ProgressCategory("game", "Game"),
    ProgressCategory("sdk", "RVL_SDK"),
    ProgressCategory("nw4r", "NW4R"),
    ProgressCategory("runtime", "Runtime and MSL"),
]
config.progress_each_module = args.verbose

if args.mode == "configure":
    generate_build(config)
elif args.mode == "progress":
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
