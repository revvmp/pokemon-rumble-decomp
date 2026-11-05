# Setup

## Prerequisites and references

Use Windows x86-64 with Universal CRT, Git and Python 3.10 or newer. Windows 10/11 provide the required runtime. The verifier and generators use Python's standard library; the pinned native toolchain is acquired by the setup command. Visual Studio and an IDA installation are not required to build.

Follow [reference identification](re/REFERENCE.md) before creating a target. Keep originals under `orig/<target>/`, outputs under `build/<target>/`, and local tool binaries under `tools/local/`. Never modify reference files to resolve a mismatch.

## Create and freeze a manifest

Copy `config/targets/template.json` to `config/targets/<target>.json`. Replace `<target>` in paths and commands with the release identifier established from evidence. Fill every metadata field and add the artifacts that the current build milestone must reproduce. `scope` must describe the compared content and exclusions. `provenance` must describe reference identification and link to findings, without private information.

Each artifact has this shape; replace the example path with an actual inventory path:

```json
{
  "path": "path/from/reference/root",
  "kind": "executable",
  "size": null,
  "sha256": null
}
```

Allowed kinds are `executable`, `data`, and `container`. Paths are relative, use forward slashes, contain no `.` or `..` components, and are unique without case distinctions. Originals and rebuilds use identical relative paths under separate roots. The template deliberately fails verification until configured.

```powershell
python tools/verify/verify.py fingerprint --manifest config/targets/<target>.json --reference-dir orig/<target>
```

Fingerprinting fills byte sizes and SHA-256 values. It refuses manifests with any existing fingerprints. Review and commit the metadata and hashes, never the original files. A digest identifies the local input; it does not independently prove authenticity. Once recorded, treat reference fingerprints as frozen.

For expanded scope, create a separately named manifest retaining all previously established entries. Fingerprint new entries in a separate unsealed manifest, then copy their records into the expanded manifest and review the diff against the inventory. Do not clear existing digests to reseal a target. A different release requires a separate target.

An original package retained solely as provenance may have its own manifest. Do not include it in an acceptance manifest unless the build is expected to reproduce it.

## Build and verify

`build.bat` at the repository root runs every step below in order: acquire tools, install ninja, configure, build, and verify. It stops at the first failure. The sections below are the individual commands.

### Acquire tools

All binaries go in ignored `tools/local/`. Sources and digests are pinned in `config/toolchain.json`; check each digest after download.

The acquisition script downloads pinned tools, rejects digest mismatches, and checks the extracted Wii/1.1 compiler and linker. It also builds the pinned Dolphin DSP assembler with portable Zig using config/dsp_assembler.json; Zig's native Thumb assembler/linker supplies the controller patch pipeline. Sources and license notices stay in ignored tools/local/. Matching cached inputs are reused, and --offline only verifies existing inputs/installations:

```powershell
python tools/setup/acquire_tools.py
python -m venv .venv
.venv/Scripts/python -m pip install ninja==1.13.0
```

The manual equivalent follows.

```powershell
New-Item -ItemType Directory -Force tools/local
curl.exe -sSfL -o tools/local/dtk.exe https://github.com/encounter/decomp-toolkit/releases/download/v1.8.4/dtk-windows-x86.exe
curl.exe -sSfL -o tools/local/objdiff-cli.exe https://github.com/encounter/objdiff/releases/download/v3.8.2/objdiff-cli-windows-x86_64.exe
curl.exe -sSfL -o tools/local/sjiswrap.exe https://github.com/encounter/sjiswrap/releases/download/v1.2.2/sjiswrap-windows-x86.exe
curl.exe -sSfL -o tools/local/binutils.zip https://github.com/encounter/gc-wii-binutils/releases/download/2.42-2/windows-x86_64.zip
curl.exe -sSfL -o tools/local/compilers.zip https://files.decomp.dev/compilers_20251118.zip
Expand-Archive tools/local/binutils.zip tools/local/binutils
Expand-Archive tools/local/compilers.zip tools/local/compilers
python -m venv .venv
.venv/Scripts/python -m pip install ninja==1.13.0
```

`configure.py` uses these local paths by default. Without them, the vendored build system downloads its pinned tags into `build/tools/`, including the x86-64 dtk, whose digest differs from the pinned x86 build (F-0006).

PowerPC assembly units use the pinned binutils powerpc-eabi-as under [D-0015](DECISIONS.md#d-0015-assembly-source-only-where-the-original-was-assembly). The AX/OS DSP sources use the separately acquired Dolphin assembler; WUD Thumb sources use the same pinned Zig installation. The OS and WUD programs are the two explicit [D-0017 exceptions](DECISIONS.md#d-0017-two-owner-approved-embedded-firmware-assembly-exceptions). After manual acquisition of the CPU tools above, also run `python tools/setup/acquire_dsp_assembler.py`. The normal build.bat/setup command already includes this step.

### Build

The normal pipeline also generates AX DSP and OS initialization images, WUD controller records, and their C includes under the selected build directory. Their rules read symbolic source and typed metadata, never original executable bytes. The DSP rule follows every nested include; the controller rule depends on all assembly/linker/manifest files and the verified Zig installation receipt. No private probe or prebuilt scratch object is required. See [final build evidence](re/findings/main-source-completion-20261104.md).

```powershell
python configure.py
.venv/Scripts/ninja
```

The first run analyzes `orig/wpse-v1/main.dol`, may rewrite `config/wpse-v1/symbols.txt` and `splits.txt`, compiles units listed in `configure.py`, links `build/wpse-v1/main.dol`, checks `config/wpse-v1/build.sha1`, and writes the objdiff report `build/wpse-v1/report.json`. Add `--map` to `configure.py` for a linker map. Review any `symbols.txt` or `splits.txt` rewrite before committing; dtk drops attributes it does not preserve.

To iterate on one unit without linking, after one full build has split the original objects:

```powershell
python tools/re/unitdiff.py <unit or source path> [--function <name>] [--all] [--json] [--no-build]
```

It accepts an `objdiff.json` unit name (`main/game/BController`, `game/BController`, `BController`) or a source path, runs `ninja` for that unit's object only, and summarizes `objdiff-cli diff` per section and per function. Rows are marked `|` for an argument mismatch, `r` for a replaced instruction, `<` for an instruction only in the original, and `>` for one only in the rebuilt object. A `Matching` claim still needs the full build and verification.

To merge an issue branch, run `python tools/collab/land.py --issue <n>` from its worktree; it loops rebase, build, verification, push, and squash merge until the merge lands on the latest `main`, then refreshes the progress images. `--dry-run` performs only the local steps. Worktrees made by `claim_issue.py worktree` start from a seeded `build/` (`tools/collab/build_seed.py`), so their first build is incremental; see [collaboration](COLLABORATION.md#working-with-llm-agents). Pull request housekeeping tools are in `tools/collab/`: `claim_issue.py overlap` warns about open pull requests that change the same files as your branch, and `stale.py` lists idle, unmerged, and overlapping pull requests; see [collaboration](COLLABORATION.md#claiming-work).

### Verify

The normal build regenerates the embedded system-message archive from the tracked editable JSON and PNG inputs under `orig/wpse-v1/reconstructed/system_message/`. It needs only Python's standard library. The archive and C initializer are generated under `build/wpse-v1/`; no original DOL bytes feed this asset rule. To inspect its standalone output and member hashes:

```powershell
python tools/assets/build_system_message.py orig/wpse-v1/reconstructed/system_message/manifest.json build/wpse-v1/assets/system_message.arc --members build/wpse-v1/assets/system_message
```

See [asset schema, provenance and validation](re/findings/system-message-archive-20261104.md). The normal `configure.py` and ninja commands remain the build entry point.

Units with a `text_sections` option in `configure.py` use `tools/build/order_text_sections.py` automatically. It copies selected inputs without changing bytes into unique basenames and writes native final-link selectors under `build/<target>/main_link/`. This avoids the linker's basename collisions while preserving source/fallback ownership. The generated objdiff unit enables `combine_text_sections` for reporting. See [native section linking](re/findings/native-section-link.md); no extra manual build command is needed.

An object with `partial_sources` can also specify `link_source_order`, naming every compiled source exactly once. Its native partial-link object remains the comparison input; the final executable consumes the individual compiler outputs in the declared order. The native section preparer groups their `.text` spans while retaining input order for exception records and data. CModel uses this to avoid the pinned final linker's failure on its partially linked custom-section object. The ordinary configure/ninja entry point handles both paths.

```powershell
python tools/verify/verify.py verify --manifest config/targets/<target>.json --reference-dir orig/<target> --build-dir build/<target> --report build/<target>/verification.json
```

Exit code `0` means all declared artifacts passed reference size and SHA-256 validation, rebuilt size and SHA-256 validation, and direct byte comparison. Exit code `1` means a mismatch or invalid input. Command-line usage errors return `2`. Reports cover only listed artifacts, not unlisted game content or source completeness. Reports are written for completed comparisons, including mismatches; invalid setup may fail before a report can be written. Always inspect the exit code and do not reuse a stale report.

For help without originals:

```powershell
python tools/verify/verify.py --help
python tools/verify/verify.py fingerprint --help
python tools/verify/verify.py verify --help
```

## Continuous integration

`.github/workflows/build.yml` runs only on demand (`gh workflow run build.yml --ref main`, or **Run workflow** in the Actions tab). It no longer runs on pushes or pull requests: it was too slow to gate parallel merges, and contributors verify locally before every merge ([collaboration](COLLABORATION.md#definition-of-done)). On a fresh Windows runner it does the following, with no caching so stale outputs cannot hide failures:

1. Acquires the pinned tools with `tools/setup/acquire_tools.py`, failing on any digest mismatch.
2. Installs ninja 1.13.0, runs `python configure.py` and `ninja`, and verifies `main.dol` against `config/targets/wpse-v1.json`. A build or verification failure fails the check.
3. Warns, without failing, when the build rewrites a tracked file such as `config/wpse-v1/symbols.txt`. `.gitattributes` checks out dtk's config files with LF line endings, so a clean build does not rewrite them on Windows.
4. Uploads `build/wpse-v1/report.json` and `verification.json` as the `reports` artifact.
5. On `main` only, renders the progress images and uploads them as the `progress-images` artifact. If they changed, it commits them to `main` as github-actions with the message `DOCS: refresh progress images`. That push does not start another run. Runs on `main` queue one at a time. A run commits only if `main` has not moved since its build, or moved only by image commits; otherwise it skips and the newer run refreshes the images.

Progress images no longer depend on CI. After each merge, `python tools/progress/refresh_images.py`, run from the merging worktree, renders them from that verified build. It pushes a `DOCS: refresh progress images` commit, built with a temporary index on top of `origin/main`, only when `main` differs from the build in nothing but `docs/images/`, and only when the last such commit on `main` is at least 60 minutes old (`--min-interval <minutes>`; `0` always refreshes). Otherwise it skips, and a later merge refreshes them. The interval keeps image commits from moving `main` under every other agent's merge loop; a maintainer can force a refresh with `--min-interval 0`. `--dry-run` renders without pushing.

To preview the images locally after a build, render them into a scratch directory rather than `docs/images/`:

```powershell
python tools/progress/progress.py render --phases config/progress.json --report build/wpse-v1/report.json --output-dir build/progress-images
```

A branch protection rule that blocks direct pushes to `main` must allow github-actions to push, or the image commit fails with a warning.

`orig/` is tracked in this private repository, so the runner checks out the originals directly. Never make the repository public while originals are tracked.

Do not make the check required: it no longer runs on pull requests.
