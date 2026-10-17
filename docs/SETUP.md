# Setup

## Prerequisites and references

Use Git and Python 3.10 or newer. The verifier uses only the Python standard library. The original compiler, linker, extractor, and build driver are not yet identified.

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

### Acquire tools

All binaries go in ignored `tools/local/`. Sources and digests are pinned in `config/toolchain.json`; check each digest after download.

The acquisition script downloads every pinned tool, rejects any digest mismatch, extracts the archives, and checks the extracted Wii/1.1 compiler and linker digests. It skips files whose digests already match, and `--offline` only verifies what is present:

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

### Build

```powershell
python configure.py
.venv/Scripts/ninja
```

The first run analyzes `orig/wpse-v1/main.dol`, may rewrite `config/wpse-v1/symbols.txt` and `splits.txt`, compiles units listed in `configure.py`, links `build/wpse-v1/main.dol`, checks `config/wpse-v1/build.sha1`, and writes the objdiff report `build/wpse-v1/report.json`. Add `--map` to `configure.py` for a linker map. Review any `symbols.txt` or `splits.txt` rewrite before committing; dtk drops attributes it does not preserve.

### Verify

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

`.github/workflows/build.yml` runs on every push to `main`, every pull request, and on demand. On a fresh Windows runner it does the following, with no caching so stale outputs cannot hide failures:

1. Acquires the pinned tools with `tools/setup/acquire_tools.py`, failing on any digest mismatch.
2. Installs ninja 1.13.0, runs `python configure.py` and `ninja`, and verifies `main.dol` against `config/targets/wpse-v1.json`. A build or verification failure fails the check.
3. Warns, without failing, when the build rewrites a tracked file such as `config/wpse-v1/symbols.txt`. `.gitattributes` checks out dtk's config files with LF line endings, so a clean build does not rewrite them on Windows.
4. Uploads `build/wpse-v1/report.json` and `verification.json` as the `reports` artifact.
5. On `main` only, renders the progress images into the `progress-images` artifact and warns when `docs/images/` is stale. A maintainer commits the refreshed images.

`orig/` is tracked in this private repository, so the runner checks out the originals directly. Never make the repository public while originals are tracked.

To make the check required, a maintainer adds the `Build and verify main.dol` status check to a branch protection rule or ruleset for `main`. Private repositories need a paid GitHub plan for this.
