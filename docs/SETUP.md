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
python tools/verify.py fingerprint --manifest config/targets/<target>.json --reference-dir orig/<target>
```

Fingerprinting fills byte sizes and SHA-256 values. It refuses manifests with any existing fingerprints. Review and commit the metadata and hashes, never the original files. A digest identifies the local input; it does not independently prove authenticity. Once recorded, treat reference fingerprints as frozen.

For expanded scope, create a separately named manifest retaining all previously established entries. Fingerprint new entries in a separate unsealed manifest, then copy their records into the expanded manifest and review the diff against the inventory. Do not clear existing digests to reseal a target. A different release requires a separate target.

An original package retained solely as provenance may have its own manifest. Do not include it in an acceptance manifest unless the build is expected to reproduce it.

## Build and verify

There is no game build command yet. Follow [toolchain investigation](re/TOOLCHAIN.md), then add the evidenced build entry point and document its invocation here and in `AGENTS.md`. Do not substitute dummy outputs for a build.

Once real rebuilt artifacts exist:

```powershell
python tools/verify.py verify --manifest config/targets/<target>.json --reference-dir orig/<target> --build-dir build/<target> --report build/<target>/verification.json
```

Exit code `0` means all declared artifacts passed reference size and SHA-256 validation, rebuilt size and SHA-256 validation, and direct byte comparison. Exit code `1` means a mismatch or invalid input. Command-line usage errors return `2`. Reports cover only listed artifacts, not unlisted game content or source completeness. Reports are written for completed comparisons, including mismatches; invalid setup may fail before a report can be written. Always inspect the exit code and do not reuse a stale report.

For help without originals:

```powershell
python tools/verify.py --help
python tools/verify.py fingerprint --help
python tools/verify.py verify --help
```
