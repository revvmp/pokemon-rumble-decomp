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

cflags_trk = [
    *(flag for flag in cflags_base if flag not in ("-inline auto", "-str reuse")),
    "-inline auto",
    "-str reuse,readonly",
    "-use_lmw_stmw on",
    "-sdata 0",
    "-sdata2 0",
    "-func_align 4",
    "-i include/MetroTRK",
    "-DMETRO_TRK",
]

RUNTIME_COMPILER = "Wii/1.1"

cflags_msl = [
    *(flag for flag in cflags_runtime if flag != "-fp_contract on"),
    "-fp_contract off",
    "-ipa file",
]

cflags_sdk = [
    *(flag for flag in cflags_base if flag not in ("-fp_contract on", "-inline auto")),
    "-inline auto,level=3",
    "-ipa file",
    "-sdata 8",
    "-sdata2 8",
]

cflags_sdk_exi = [*("-O3" if flag == "-O4,p" else flag for flag in cflags_sdk), "-func_align 16"]

SDK_COMPILER = "Wii/1.0"

cflags_hbm = [
    *(flag for flag in cflags_base if flag not in ("-RTTI off", "-fp_contract on")),
    "-lang=c++",
    "-ipa file",
    "-fp_contract off",
    "-func_align 16",
    "-sdata 0",
    "-sdata2 0",
    "-RTTI on",
]

HBM_COMPILER = "Wii/1.0a"

cflags_nw4r = [
    *(flag for flag in cflags_base if flag != "-fp_contract on"),
    "-fp_contract off",
    "-ipa file",
]

NW4R_COMPILER = "Wii/1.1"

cflags_revoex = [*(flag for flag in cflags_base if flag != "-inline auto"), "-inline auto,deferred"]

REVOEX_COMPILER = "GC/3.0a5.2"

cflags_tmcjpeg = [*cflags_base, "-func_align 4", "-use_lmw_stmw on", "-str reuse,readonly"]

TMCJPEG_COMPILER = "Wii/1.0"

cflags_game = [
    *(flag for flag in cflags_base if flag not in ("-Cpp_exceptions off", "-RTTI off", "-fp_contract on")),
    "-Cpp_exceptions on",
    "-RTTI on",
    "-fp_contract off",
    "-ipa file",
]

GAME_COMPILER = "Wii/1.1"

BTE_COMPILER = "GC/3.0a3"
NDEV_COMPILER = "GC/3.0a3"

config.libs = [
    library(
        "Game",
        GAME_COMPILER,
        cflags_game,
        "game",
        [
            Object(Matching, "game/BController.cpp"),
            Object(NonMatching, "game/CDlgCollection.cpp"),
            Object(NonMatching, "game/CDlgInfo.cpp"),
            Object(NonMatching, "game/CDlgPiiList.cpp"),
        ],
    ),
    library(
        "RevoEX",
        REVOEX_COMPILER,
        cflags_revoex,
        "sdk",
        [
            Object(Matching, "RevoEX/net/NETVersion.c"),
            Object(Matching, "RevoEX/net/wireless_macaddr.c"),
            Object(Matching, "RevoEX/net/sha1.c"),
            Object(Matching, "RVL_SDK/ncd/ncdsystem.c"),
        ],
    ),
    library(
        "RVL_SDK.CNT",
        "Wii/1.0",
        cflags_sdk,
        "sdk",
        [Object(Matching, "RVL_SDK/cnt/cnt.c")],
    ),
    library(
        "homebuttonLib",
        HBM_COMPILER,
        cflags_hbm,
        "sdk",
        [
            Object(Matching, "RVL_SDK/hbm/HBMFrameController.cpp"),
            Object(Matching, "RVL_SDK/hbm/HBMAnmController.cpp"),
            Object(Matching, "RVL_SDK/hbm/HBMGUIManager.cpp"),
            Object(Matching, "RVL_SDK/hbm/HBMController.cpp"),
            Object(Matching, "RVL_SDK/hbm/HBMRemoteSpk.cpp"),
            Object(Matching, "RVL_SDK/hbm/HBMAxSound.cpp"),
            Object(Matching, "RVL_SDK/hbm/HBMCommon.cpp"),
            Object(Matching, "RVL_SDK/hbm/HBMBase.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_animation.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_arcResourceAccessor.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_bounding.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_common.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_drawInfo.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_group.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_layout.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_material.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_pane.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_picture.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_resourceAccessor.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_textBox.cpp"),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_window.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/math/math_triangular.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_binaryFileFormat.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_CharStrmReader.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_CharWriter.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_Font.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_LinkList.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_list.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_ResFont.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_ResFontBase.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_TagProcessorBase.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_TextWriterBase.cpp"),
            Object(Matching, "RVL_SDK/hbm/mix.c"),
            Object(Matching, "RVL_SDK/hbm/syn.c"),
            Object(Matching, "RVL_SDK/hbm/synctrl.c"),
            Object(Matching, "RVL_SDK/hbm/synenv.c"),
            Object(Matching, "RVL_SDK/hbm/synmix.c"),
            Object(Matching, "RVL_SDK/hbm/synpitch.c"),
            Object(Matching, "RVL_SDK/hbm/synsample.c"),
            Object(Matching, "RVL_SDK/hbm/synvoice.c"),
            Object(Matching, "RVL_SDK/hbm/seq.c"),
        ],
    ),
    library(
        "TMC_JPEG",
        TMCJPEG_COMPILER,
        cflags_tmcjpeg,
        "sdk",
        [
            Object(Matching, "TMC_JPEG/tmcjpeg.c"),
            Object(Matching, "TMC_JPEG/tmcjpeg_block.c"),
            Object(NonMatching, "TMC_JPEG/tmcjpeg_encoder.c"),
            Object(Matching, "TMC_JPEG/tmcjpeg_huffman.c"),
            Object(Matching, "TMC_JPEG/tmcjpeg_quant.c"),
            Object(NonMatching, "TMC_JPEG/tmcjpeg_stream.c"),
            Object(NonMatching, "TMC_JPEG/tmcjpeg_rgba8.c"),
            Object(NonMatching, "TMC_JPEG/tmcjpeg_rgb565.c"),
        ],
    ),
    library(
        "RVL_SDK.FA",
        SDK_COMPILER,
        cflags_sdk,
        "sdk",
        [
            Object(Matching, "RVL_SDK/fa/pf_clib.c"),
            Object(Matching, "RVL_SDK/fa/pf_code.c"),
            Object(Matching, "RVL_SDK/fa/pf_service.c"),
            Object(Matching, "RVL_SDK/fa/pf_str.c"),
            Object(Matching, "RVL_SDK/fa/pdm_bpb.c"),
            Object(Matching, "RVL_SDK/fa/pdm_disk.c"),
            Object(NonMatching, "RVL_SDK/fa/pdm_mbr.c"),
            Object(Matching, "RVL_SDK/fa/pdm_partition.c"),
            Object(Matching, "RVL_SDK/fa/pf_cache.c"),
            Object(Matching, "RVL_SDK/fa/pf_cluster.c"),
            Object(Matching, "RVL_SDK/fa/pf_dir.c"),
            Object(Matching, "RVL_SDK/fa/pf_entry.c"),
            Object(NonMatching, "RVL_SDK/fa/pf_entry_iterator.c"),
            Object(Matching, "RVL_SDK/fa/pf_api_util.c"),
            Object(Matching, "RVL_SDK/fa/pf_chdir.c"),
            Object(Matching, "RVL_SDK/fa/pf_kernel.c"),
            Object(Matching, "RVL_SDK/fa/nand_drv.c"),
        ],
    ),
    library(
        "NW4R.ef",
        NW4R_COMPILER,
        cflags_nw4r,
        "nw4r",
        [
            Object(Matching, "nw4r/ef/ef_draworder.cpp"),
            Object(Matching, "nw4r/ef/ef_effect.cpp"),
            Object(Matching, "nw4r/ef/ef_effectsystem.cpp"),
            Object(Matching, "nw4r/ef/ef_emitter.cpp"),
            Object(Matching, "nw4r/ef/ef_particle.cpp"),
            Object(NonMatching, "nw4r/ef/ef_particlemanager.cpp"),
            Object(Matching, "nw4r/ef/ef_resource.cpp"),
            Object(Matching, "nw4r/ef/ef_util.cpp"),
            Object(Matching, "nw4r/ef/ef_handle.cpp"),
            Object(Matching, "nw4r/ef/ef_emitterform.cpp"),
            Object(Matching, "nw4r/ef/ef_creationqueue.cpp"),
            Object(Matching, "nw4r/ef/ef_emform.cpp"),
            Object(Matching, "nw4r/ef/ef_point.cpp"),
            Object(Matching, "nw4r/ef/ef_line.cpp"),
            Object(Matching, "nw4r/ef/ef_disc.cpp"),
            Object(Matching, "nw4r/ef/ef_sphere.cpp"),
            Object(Matching, "nw4r/ef/ef_cylinder.cpp"),
            Object(Matching, "nw4r/ef/ef_torus.cpp"),
            Object(Matching, "nw4r/ef/ef_cube.cpp"),
            Object(Matching, "nw4r/ef/ef_drawstrategybuilder.cpp"),
            Object(Matching, "nw4r/ef/ef_drawstrategyimpl.cpp"),
            Object(Matching, "nw4r/ef/ef_drawbillboardstrategy.cpp"),
            Object(NonMatching, "nw4r/ef/ef_drawdirectionalstrategy.cpp"),
            Object(NonMatching, "nw4r/ef/ef_drawfreestrategy.cpp"),
            Object(Matching, "nw4r/ef/ef_drawlinestrategy.cpp"),
            Object(Matching, "nw4r/ef/ef_drawpointstrategy.cpp"),
            Object(NonMatching, "nw4r/ef/ef_drawsmoothstripestrategy.cpp"),
        ],
    ),
    library(
        "NW4R.g3d",
        NW4R_COMPILER,
        cflags_nw4r,
        "nw4r",
        [
            Object(Matching, "nw4r/g3d/res/g3d_rescommon.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resdict.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resfile.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resmdl.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resshp.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_restev.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resmat.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resvtx.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_restex.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resnode.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanm.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmvis.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmclr.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmtexpat.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmtexsrt.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmchr.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_reslightset.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmamblight.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmlight.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmfog.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmcamera.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmscn.cpp"),
            Object(Matching, "nw4r/g3d/res/g3d_resanmshp.cpp"),
            Object(Matching, "nw4r/g3d/g3d_transform.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmvis.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmclr.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmtexpat.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmtexsrt.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmchr.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmshp.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmscn.cpp"),
            Object(Matching, "nw4r/g3d/g3d_obj.cpp"),
            Object(Matching, "nw4r/g3d/g3d_anmobj.cpp"),
            Object(Matching, "nw4r/g3d/g3d_scnmdl.cpp"),
            Object(Matching, "nw4r/g3d/platform/g3d_gpu.cpp"),
            Object(Matching, "nw4r/g3d/platform/g3d_cpu.cpp"),
            Object(Matching, "nw4r/g3d/g3d_state.cpp"),
            Object(Matching, "nw4r/g3d/g3d_draw1mat1shp.cpp"),
            Object(Matching, "nw4r/g3d/g3d_light.cpp"),
            Object(Matching, "nw4r/g3d/g3d_calcvtx.cpp"),
            Object(Matching, "nw4r/g3d/g3d_calcview.cpp"),
            Object(Matching, "nw4r/g3d/g3d_draw.cpp"),
            Object(Matching, "nw4r/g3d/g3d_dcc.cpp"),
            Object(Matching, "nw4r/g3d/g3d_workmem.cpp"),
            Object(Matching, "nw4r/g3d/g3d_calcworld.cpp"),
            Object(Matching, "nw4r/g3d/g3d_camera.cpp"),
            Object(Matching, "nw4r/g3d/dcc/g3d_basic.cpp"),
            Object(Matching, "nw4r/g3d/dcc/g3d_maya.cpp"),
            Object(Matching, "nw4r/g3d/g3d_scnmdlsmpl.cpp"),
            Object(Matching, "nw4r/g3d/dcc/g3d_xsi.cpp"),
            Object(Matching, "nw4r/g3d/dcc/g3d_3dsmax.cpp"),
            Object(Matching, "nw4r/g3d/g3d_scnobj.cpp"),
            Object(Matching, "nw4r/g3d/g3d_scnroot.cpp"),
            Object(Matching, "nw4r/g3d/g3d_calcmaterial.cpp"),
            Object(Matching, "nw4r/g3d/g3d_init.cpp"),
            Object(Matching, "nw4r/g3d/g3d_fog.cpp"),
        ],
    ),
    library(
        "NW4R.lyt",
        NW4R_COMPILER,
        cflags_nw4r,
        "nw4r",
        [
            Object(Matching, "nw4r/lyt/lyt_init.cpp"),
            Object(Matching, "nw4r/lyt/lyt_pane.cpp"),
            Object(Matching, "nw4r/lyt/lyt_group.cpp"),
            Object(Matching, "nw4r/lyt/lyt_layout.cpp"),
            Object(Matching, "nw4r/lyt/lyt_picture.cpp"),
            Object(Matching, "nw4r/lyt/lyt_window.cpp"),
            Object(Matching, "nw4r/lyt/lyt_animation.cpp"),
            Object(Matching, "nw4r/lyt/lyt_resourceAccessor.cpp"),
            Object(Matching, "nw4r/lyt/lyt_arcResourceAccessor.cpp"),
            Object(Matching, "nw4r/lyt/lyt_common.cpp"),
            Object(Matching, "nw4r/lyt/lyt_util.cpp"),
        ],
    ),
    library(
        "NW4R.db",
        NW4R_COMPILER,
        cflags_nw4r,
        "nw4r",
        [Object(Matching, "nw4r/db/db_DbgPrintBase.cpp")],
    ),
    library(
        "NW4R.math",
        NW4R_COMPILER,
        cflags_nw4r,
        "nw4r",
        [
            Object(Matching, "nw4r/math/math_arithmetic.cpp"),
            Object(Matching, "nw4r/math/math_triangular.cpp"),
            Object(Matching, "nw4r/math/math_types.cpp"),
            Object(Matching, "nw4r/math/math_geometry.cpp"),
        ],
    ),
    library(
        "NW4R.snd",
        NW4R_COMPILER,
        cflags_nw4r,
        "nw4r",
        [
            Object(Matching, "nw4r/snd/snd_AxManager.cpp"),
            Object(Matching, "nw4r/snd/snd_AxVoice.cpp"),
            Object(Matching, "nw4r/snd/snd_AxVoiceManager.cpp"),
            Object(Matching, "nw4r/snd/snd_AxfxImpl.cpp"),
            Object(Matching, "nw4r/snd/snd_Bank.cpp"),
            Object(Matching, "nw4r/snd/snd_BankFile.cpp"),
            Object(Matching, "nw4r/snd/snd_BasicPlayer.cpp"),
            Object(Matching, "nw4r/snd/snd_BasicSound.cpp"),
            Object(Matching, "nw4r/snd/snd_BiquadFilterPreset.cpp"),
            Object(Matching, "nw4r/snd/snd_Channel.cpp"),
            Object(Matching, "nw4r/snd/snd_DisposeCallbackManager.cpp"),
            Object(Matching, "nw4r/snd/snd_debug.cpp"),
            Object(Matching, "nw4r/snd/snd_EnvGenerator.cpp"),
            Object(Matching, "nw4r/snd/snd_ExternalSoundPlayer.cpp"),
            Object(Matching, "nw4r/snd/snd_FxBase.cpp"),
            Object(Matching, "nw4r/snd/snd_FxChorus.cpp"),
            Object(Matching, "nw4r/snd/snd_FxDelay.cpp"),
            Object(Matching, "nw4r/snd/snd_FxReverbStd.cpp"),
            Object(Matching, "nw4r/snd/snd_InstancePool.cpp"),
            Object(Matching, "nw4r/snd/snd_Lfo.cpp"),
            Object(Matching, "nw4r/snd/snd_MemorySoundArchive.cpp"),
            Object(Matching, "nw4r/snd/snd_MidiSeqPlayer.cpp"),
            Object(Matching, "nw4r/snd/snd_MmlParser.cpp"),
            Object(Matching, "nw4r/snd/snd_MmlSeqTrack.cpp"),
            Object(Matching, "nw4r/snd/snd_MmlSeqTrackAllocator.cpp"),
            Object(Matching, "nw4r/snd/snd_NandSoundArchive.cpp"),
            Object(Matching, "nw4r/snd/snd_PlayerHeap.cpp"),
            Object(Matching, "nw4r/snd/snd_RemoteSpeaker.cpp"),
            Object(Matching, "nw4r/snd/snd_RemoteSpeakerManager.cpp"),
            Object(Matching, "nw4r/snd/snd_SeqFile.cpp"),
            Object(Matching, "nw4r/snd/snd_SeqPlayer.cpp"),
            Object(NonMatching, "nw4r/snd/snd_SeqSound.cpp"),
            Object(Matching, "nw4r/snd/snd_SeqSoundHandle.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundHandle.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundPlayer.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundStartable.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundSystem.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundThread.cpp"),
            Object(Matching, "nw4r/snd/snd_StrmChannel.cpp"),
            Object(Matching, "nw4r/snd/snd_StrmSound.cpp"),
            Object(Matching, "nw4r/snd/snd_StrmSoundHandle.cpp"),
            Object(Matching, "nw4r/snd/snd_Task.cpp"),
            Object(Matching, "nw4r/snd/snd_TaskManager.cpp"),
            Object(Matching, "nw4r/snd/snd_TaskThread.cpp"),
            Object(Matching, "nw4r/snd/snd_StrmFile.cpp"),
            Object(Matching, "nw4r/snd/snd_Sound3DEngine.cpp"),
            Object(Matching, "nw4r/snd/snd_Sound3DListener.cpp"),
            Object(Matching, "nw4r/snd/snd_Sound3DManager.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundActor.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundArchive.cpp"),
            Object(Matching, "nw4r/snd/snd_SoundArchiveFile.cpp"),
            Object(Matching, "nw4r/snd/snd_SeqTrack.cpp"),
            Object(Matching, "nw4r/snd/snd_Sound3DActor.cpp"),
            Object(Matching, "nw4r/snd/snd_Sound3DCalculator.cpp"),
            Object(Matching, "nw4r/snd/snd_Voice.cpp"),
            Object(Matching, "nw4r/snd/snd_VoiceManager.cpp"),
            Object(Matching, "nw4r/snd/snd_Util.cpp"),
            Object(Matching, "nw4r/snd/snd_WaveArchive.cpp"),
            Object(Matching, "nw4r/snd/snd_WaveFile.cpp", cflags=[*(flag for flag in cflags_nw4r if flag != "-ipa file"), "-ipa off"]),
            Object(Matching, "nw4r/snd/snd_WaveSound.cpp"),
            Object(Matching, "nw4r/snd/snd_WaveSoundHandle.cpp"),
            Object(Matching, "nw4r/snd/snd_WsdFile.cpp"),
            Object(Matching, "nw4r/snd/snd_WsdPlayer.cpp"),
            Object(Matching, "nw4r/snd/snd_adpcm.cpp", extra_cflags=["-func_align 4"]),
        ],
    ),
    library(
        "NW4R.ut",
        NW4R_COMPILER,
        cflags_nw4r,
        "nw4r",
        [
            Object(Matching, "nw4r/ut/ut_list.cpp"),
            Object(Matching, "nw4r/ut/ut_LinkList.cpp"),
            Object(Matching, "nw4r/ut/ut_binaryFileFormat.cpp"),
            Object(Matching, "nw4r/ut/ut_CharStrmReader.cpp"),
            Object(Matching, "nw4r/ut/ut_TagProcessorBase.cpp"),
            Object(Matching, "nw4r/ut/ut_IOStream.cpp"),
            Object(Matching, "nw4r/ut/ut_FileStream.cpp"),
            Object(Matching, "nw4r/ut/ut_DvdFileStream.cpp"),
            Object(Matching, "nw4r/ut/ut_NandFileStream.cpp"),
            Object(Matching, "nw4r/ut/ut_LockedCache.cpp"),
            Object(Matching, "nw4r/ut/ut_Font.cpp"),
            Object(Matching, "nw4r/ut/ut_RomFont.cpp"),
            Object(Matching, "nw4r/ut/ut_ResFontBase.cpp"),
            Object(Matching, "nw4r/ut/ut_ResFont.cpp"),
            Object(NonMatching, "nw4r/ut/ut_ArchiveFontBase.cpp"),
            Object(Matching, "nw4r/ut/ut_ArchiveFont.cpp"),
            Object(Matching, "nw4r/ut/ut_PairFont.cpp"),
        ],
    ),
    library(
        "RVL_SDK",
        SDK_COMPILER,
        cflags_sdk,
        "sdk",
        [
            Object(Matching, "RVL_SDK/base/PPCArch.c"),
            Object(Matching, "RVL_SDK/os/OS.c"),
            Object(Matching, "RVL_SDK/os/OSAlarm.c"),
            Object(Matching, "RVL_SDK/os/OSAlloc.c"),
            Object(Matching, "RVL_SDK/os/OSArena.c"),
            Object(Matching, "RVL_SDK/os/OSAudioSystem.c"),
            Object(Matching, "RVL_SDK/os/OSCache.c"),

            Object(Matching, "RVL_SDK/os/OSContext.c"),
            Object(Matching, "RVL_SDK/os/OSError.c"),
            Object(Matching, "RVL_SDK/os/OSExec.c"),
            Object(Matching, "RVL_SDK/os/OSFatal.c"),
            Object(Matching, "RVL_SDK/os/OSFont.c"),
            Object(Matching, "RVL_SDK/os/OSInterrupt.c"),
            Object(Matching, "RVL_SDK/os/OSLink.c"),
            Object(Matching, "RVL_SDK/os/OSMessage.c"),
            Object(Matching, "RVL_SDK/os/OSMemory.c"),
            Object(Matching, "RVL_SDK/os/OSMutex.c"),
            Object(Matching, "RVL_SDK/os/OSReboot.c"),
            Object(Matching, "RVL_SDK/os/OSReset.c"),
            Object(Matching, "RVL_SDK/os/OSRtc.c"),
            Object(Matching, "RVL_SDK/os/OSSemaphore.c"),
            Object(Matching, "RVL_SDK/os/OSSync.c"),
            Object(Matching, "RVL_SDK/os/OSThread.c"),
            Object(Matching, "RVL_SDK/os/OSTime.c"),
            Object(Matching, "RVL_SDK/os/OSUtf.c"),
            Object(Matching, "RVL_SDK/os/OSIpc.c"),
            Object(Matching, "RVL_SDK/os/OSStateTM.c"),
            Object(Matching, "RVL_SDK/os/OSPlayRecord.c"),
            Object(Matching, "RVL_SDK/os/OSStateFlags.c"),
            Object(Matching, "RVL_SDK/os/OSNet.c"),
            Object(Matching, "RVL_SDK/os/OSNandbootInfo.c"),
            Object(Matching, "RVL_SDK/os/OSPlayTime.c"),
            Object(Matching, "RVL_SDK/os/OSCrc.c"),
            Object(Matching, "RVL_SDK/os/OSLaunch.c"),
            Object(Matching, "RVL_SDK/os/__start.c"),
            Object(Matching, "RVL_SDK/os/__ppc_eabi_init.c"),
            Object(Matching, "RVL_SDK/exi/EXIBios.c", cflags=cflags_sdk_exi),
            Object(Matching, "RVL_SDK/exi/EXIUart.c"),
            Object(Matching, "RVL_SDK/exi/EXICommon.c"),
            Object(Matching, "RVL_SDK/si/SIBios.c"),
            Object(Matching, "RVL_SDK/si/SISamplingRate.c"),
            Object(Matching, "RVL_SDK/db/db.c"),
            Object(Matching, "RVL_SDK/vi/vi.c"),
            Object(NonMatching, "RVL_SDK/vi/i2c.c"),
            Object(Matching, "RVL_SDK/vi/vi3in1.c"),
            Object(Matching, "RVL_SDK/mtx/mtx.c"),
            Object(Matching, "RVL_SDK/mtx/mtxvec.c"),
            Object(Matching, "RVL_SDK/mtx/mtx44.c"),
            Object(Matching, "RVL_SDK/mtx/vec.c"),
            Object(Matching, "RVL_SDK/gx/GXInit.c"),
            Object(Matching, "RVL_SDK/gx/GXFifo.c"),
            Object(Matching, "RVL_SDK/gx/GXAttr.c"),
            Object(Matching, "RVL_SDK/gx/GXMisc.c"),
            Object(Matching, "RVL_SDK/gx/GXGeometry.c"),
            Object(Matching, "RVL_SDK/gx/GXFrameBuf.c"),
            Object(Matching, "RVL_SDK/gx/GXLight.c"),
            Object(Matching, "RVL_SDK/gx/GXTexture.c"),
            Object(Matching, "RVL_SDK/gx/GXBump.c"),
            Object(Matching, "RVL_SDK/gx/GXTev.c"),
            Object(Matching, "RVL_SDK/gx/GXPixel.c"),
            Object(Matching, "RVL_SDK/gx/GXDisplayList.c"),
            Object(Matching, "RVL_SDK/gx/GXTransform.c"),
            Object(Matching, "RVL_SDK/gx/GXPerf.c"),
            Object(Matching, "RVL_SDK/dvd/dvdfs.c"),
            Object(Matching, "RVL_SDK/dvd/dvd.c"),
            Object(Matching, "RVL_SDK/dvd/dvdqueue.c"),
            Object(Matching, "RVL_SDK/dvd/dvderror.c"),
            Object(Matching, "RVL_SDK/dvd/dvdidutils.c"),
            Object(Matching, "RVL_SDK/dvd/dvdFatal.c"),
            Object(Matching, "RVL_SDK/dvd/dvdDeviceError.c"),
            Object(Matching, "RVL_SDK/ai/ai.c"),
            Object(Matching, "RVL_SDK/ax/AX.c"),
            Object(Matching, "RVL_SDK/ax/AXAlloc.c"),
            Object(Matching, "RVL_SDK/ax/AXAux.c"),
            Object(Matching, "RVL_SDK/ax/AXCL.c"),
            Object(Matching, "RVL_SDK/ax/AXOut.c"),
            Object(Matching, "RVL_SDK/ax/AXSPB.c"),
            Object(Matching, "RVL_SDK/ax/AXVPB.c"),
            Object(Matching, "RVL_SDK/ax/AXProf.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHi.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXDelayExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbStdExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXChorusExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXLfoTable.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXSrcCoef.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXHooks.c"),
            Object(Matching, "RVL_SDK/mem/mem_heapCommon.c"),
            Object(Matching, "RVL_SDK/mem/mem_expHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_frameHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_allocator.c"),
            Object(Matching, "RVL_SDK/mem/mem_list.c"),
            Object(Matching, "RVL_SDK/cx/CXStreamingUncompression.c"),
            Object(Matching, "RVL_SDK/cx/CXUncompression.c"),
            Object(Matching, "RVL_SDK/cx/CXSecureUncompression.c"),
            Object(Matching, "RVL_SDK/dsp/dsp.c"),
            Object(Matching, "RVL_SDK/dsp/dsp_debug.c"),
            Object(Matching, "RVL_SDK/dsp/dsp_task.c"),
            Object(Matching, "RVL_SDK/nand/nand.c"),
            Object(Matching, "RVL_SDK/nand/NANDOpenClose.c"),
            Object(Matching, "RVL_SDK/nand/NANDCore.c"),
            Object(Matching, "RVL_SDK/nand/NANDSecret.c"),
            Object(Matching, "RVL_SDK/nand/NANDCheck.c"),
            Object(Matching, "RVL_SDK/nand/NANDLogging.c"),
            Object(Matching, "RVL_SDK/sc/scsystem.c"),
            Object(Matching, "RVL_SDK/sc/scapi.c"),
            Object(Matching, "RVL_SDK/sc/scapi_prdinfo.c"),
            Object(Matching, "RVL_SDK/wenc/wenc.c", mw_version="GC/3.0a5.2", extra_cflags=["-func_align 4"]),
            Object(Matching, "RVL_SDK/enc/encutility.c"),
            Object(Matching, "RVL_SDK/enc/encunicode.c"),
            Object(Matching, "RVL_SDK/enc/encjapanese.c"),
            Object(Matching, "RVL_SDK/enc/enclatin.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/enc/encconvert.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/enc/encchinese.c"),
            Object(Matching, "RVL_SDK/enc/enckorean.c"),
            Object(Matching, "RVL_SDK/arc/arc.c"),
            Object(Matching, "RVL_SDK/esp/esp.c"),
            Object(Matching, "RVL_SDK/ipc/ipcMain.c"),
            Object(Matching, "RVL_SDK/ipc/ipcclt.c"),
            Object(Matching, "RVL_SDK/ipc/memory.c"),
            Object(Matching, "RVL_SDK/ipc/ipcProfile.c"),
            Object(Matching, "RVL_SDK/fs/fs.c"),
            Object(Matching, "RVL_SDK/pad/Padclamp.c"),
            Object(Matching, "RVL_SDK/pad/Pad.c"),
            Object(Matching, "RVL_SDK/wpad/WPAD.c"),
            Object(Matching, "RVL_SDK/wpad/WPADHIDParser.c"),
            Object(Matching, "RVL_SDK/wpad/WPADEncrypt.c"),
            Object(Matching, "RVL_SDK/wpad/WPADMem.c"),
            Object(Matching, "RVL_SDK/wpad/debug_msg.c"),
            Object(Matching, "RVL_SDK/kpad/KPAD.c"),
            Object(Matching, "RVL_SDK/euart/euart.c"),
            Object(Matching, "RVL_SDK/usb/usb.c"),
            Object(Matching, "RVL_SDK/wud/WUD.c"),
            Object(Matching, "RVL_SDK/wud/WUDHidHost.c"),
            Object(Matching, "RVL_SDK/wud/debug_msg.c"),
            Object(Matching, "RVL_SDK/bte/gki_buffer.c", cflags=[*cflags_base, "-ipa file"]),
            Object(Matching, "RVL_SDK/bte/gki_time.c", cflags=[*cflags_base, "-ipa file"]),
            Object(Matching, "RVL_SDK/bte/gki_ppc.c", cflags=[*cflags_base, "-ipa file"]),
            Object(Matching, "RVL_SDK/bte/hcisu_h2.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/uusb_ppc.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/bte_hcisu.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/bte_init.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/bte_logmsg.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/bte_main.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/btu_task1.c", cflags=cflags_base),
            Object(Matching, "RVL_SDK/bte/bd.c", cflags=[*cflags_base, "-func_align 4"]),
            Object(Matching, "RVL_SDK/bte/bta_sys_conn.c", cflags=[*cflags_base, "-func_align 4"]),
            Object(Matching, "RVL_SDK/bte/bta_sys_main.c", cflags=[*cflags_base, "-func_align 4"]),
            Object(Matching, "RVL_SDK/bte/ptim.c", cflags=[*cflags_base, "-func_align 4"]),
            Object(Matching, "RVL_SDK/bte/utl.c", mw_version="GC/3.0a3", cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_dm_act.c", mw_version="GC/3.0a3", cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_dm_api.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_dm_main.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_dm_pm.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_hh_act.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_hh_api.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_hh_main.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/bta_hh_utils.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/btm_acl.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/btm_dev.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/btm_devctl.c", mw_version=BTE_COMPILER, cflags=cflags_sdk),
            Object(Matching, "RVL_SDK/bte/btm_discovery.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/btm_inq.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/btm_main.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/btm_pm.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/btm_sco.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/btm_sec.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/btu_hcif.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/btu_init.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/wbt_ext.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/gap_api.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/gap_conn.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/gap_utils.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/hcicmds.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/hidd_api.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/hidd_conn.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/hidd_mgmt.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/hidd_pm.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/hidh_api.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/hidh_conn.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/l2c_api.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/l2c_csm.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/l2c_link.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/l2c_main.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/l2c_utils.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/port_api.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/port_rfc.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/port_utils.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/rfc_l2cap_if.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/rfc_mx_fsm.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/rfc_port_fsm.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/rfc_port_if.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/rfc_ts_frames.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/rfc_utils.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/sdp_api.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/sdp_db.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/sdp_discovery.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/sdp_main.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/sdp_server.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/bte/sdp_utils.c", mw_version=BTE_COMPILER),
            Object(Matching, "RVL_SDK/tpl/TPL.c"),
            Object(Matching, "RVL_SDK/gd/GDBase.c"),
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
        [
            Object(Matching, "Runtime/__mem.c"),
            Object(Matching, "Runtime/__va_arg.c"),
            Object(Matching, "Runtime/global_destructor_chain.c"),
            Object(Matching, "Runtime/NMWException.cpp", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "Runtime/ptmf.c"),
            Object(Matching, "Runtime/MWRTTI.cpp", extra_cflags=["-Cpp_exceptions on", "-RTTI on"]),
            Object(Matching, "Runtime/runtime.c"),
            Object(Matching, "Runtime/__init_cpp_exceptions.cpp"),
            Object(Matching, "Runtime/Gecko_ExceptionPPC.cpp", extra_cflags=["-Cpp_exceptions on", "-RTTI on"]),
        ],
    ),
    library(
        "MSL_C",
        "Wii/1.1",
        cflags_msl,
        "runtime",
        [
            Object(Matching, "MSL_C/alloc.c"),
            Object(Matching, "MSL_C/ansi_files.c"),
            Object(Matching, "MSL_C/ansi_fp.c"),
            Object(Matching, "MSL_C/arith.c"),
            Object(Matching, "MSL_C/buffer_io.c"),
            Object(Matching, "MSL_C/direct_io.c"),
            Object(Matching, "MSL_C/wcstoul.c"),
            Object(Matching, "MSL_C/wmem.c"),
            Object(Matching, "MSL_C/wscanf.c"),
            Object(Matching, "MSL_C/wstring.c"),
            Object(Matching, "MSL_C/wchar_io.c"),
            Object(Matching, "MSL_C/uart_console_io_gcn.c"),
            Object(Matching, "MSL_C/abort_exit_ppc_eabi.c"),
            Object(Matching, "MSL_C/math_sun.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_asin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_atan2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_exp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_fmod.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_log.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_log10.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_pow.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_rem_pio2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_cos.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_rem_pio2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_sin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_tan.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_atan.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_ceil.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_copysign.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_cos.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_floor.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_frexp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_ldexp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_modf.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_sin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_tan.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_asin.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_atan2.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_exp.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_fmod.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_log.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_log10.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_pow.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_sqrt.c"),
            Object(Matching, "MSL_C/PPC_EABI/SRC/math_ppc.c"),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_sqrt.c"),
            Object(Matching, "MSL_C/extras.c"),
            Object(Matching, "MSL_C/wide_decimal.c"),
        ],
    ),
    library(
        "MSL_CPP",
        RUNTIME_COMPILER,
        cflags_msl,
        "runtime",
        [
            Object(Matching, "MSL_CPP/msl_thread.cpp", extra_cflags=["-Cpp_exceptions on"]),
        ],
    ),
    library(
        "MetroTRK",
        "Wii/1.1",
        cflags_trk,
        "runtime",
        [
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.c"),
            Object(Matching, "MetroTRK/gamedev/cust_connection/cc/exi2/GCN/EXI2_GDEV_GCN/main.c", extra_cflags=["-sdata 8"]),
            Object(Matching, "MetroTRK/gamedev/cust_connection/utils/gc/MWCriticalSection_gc.c"),
            Object(Matching, "MetroTRK/gamedev/cust_connection/utils/common/CircleBuffer.c"),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Processor/ppc/Generic/flush_cache.c"),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/main_TRK.c", extra_cflags=["-sdata 8"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/mainloop.c", extra_cflags=["-sdata 8"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/mem_TRK.c"),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/dispatch.c"),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c", extra_cflags=["-sdata 8", "-sdata2 8"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk_glue.c", extra_cflags=["-sdata 8", "-sdata2 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/notify.c"),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/nubevent.c", extra_cflags=["-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/nubinit.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/serpoll.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/string_TRK.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/support.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Os/dolphin/targcont.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Processor/ppc/Generic/mpc_7xx_603e.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/msg.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/msgbuf.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Portable/msghndlr.c", extra_cflags=["-sdata 8", "-str reuse,pool,noreadonly"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Export/mslsupp.c", extra_cflags=["-sdata 8"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c", mw_version="Wii/1.0a", extra_cflags=["-sdata 8"]),
            Object(Matching, "MetroTRK/debugger/embedded/MetroTRK/Os/dolphin/target_options.c", extra_cflags=["-sdata 8"]),
        ],
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
