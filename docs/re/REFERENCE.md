# Identify the original release

1. Preserve an untouched local original and record its acquisition provenance without private information.
2. Establish title, region, revision, and release identifiers from observable metadata. A filename is insufficient. If a revision is unlabeled, record the observed identifiers and precise uncertainty instead of inventing a revision number.
3. Choose a stable target identifier based on observed identity and add evidence to `FINDINGS.md`.
4. Inventory the original container, executable modules, and data in `INVENTORY.md`. Record extraction commands, tool versions, binary SHA-256 values, and acquisition sources. Never treat packaged and extracted bytes as interchangeable.
5. Define the first comparison scope explicitly. Retain the original package fingerprint as provenance even when the first reconstruction milestone covers only executable artifacts.
6. Configure and freeze the manifest using [setup](../SETUP.md). Review the inventory against later expanded acceptance manifests.

The original repository ignored a file named `Pokemon.Rumble.USA.WiiWare.wad`. This is a filename hint only, not evidence of region, revision, authenticity, or available original bytes.
