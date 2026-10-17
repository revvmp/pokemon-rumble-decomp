# Findings

Use a stable identifier such as `F-0001` for each entry. Record target, manifest, artifact path and digest, address or file offset, direct observation, exact inspection procedure, tool version, interpretation, confidence, alternatives, affected source or configuration, and next validation step.

Keep facts separate from hypotheses. Mark overturned conclusions superseded and link the replacement rather than silently erasing evidence history. Do not embed proprietary code dumps or assets in this record.

## F-0001: WAD container metadata

- **Target:** not yet selected. **Manifest:** none yet.
- **Artifact:** local file named `Pokemon.Rumble.USA.WiiWare.wad`, 27,330,304 bytes, SHA-256 `3edf8518c33ea03a04a0c519ae586a29e9b40a32327be72623277773ac88f680`. Provenance not yet documented.
- **Procedure:** read-only parse of unencrypted structures with an ad hoc standard-library Python 3.14.2 script: the WAD header, ticket title ID at ticket offset `0x1DC`, and TMD fields at offsets `0x184` (IOS), `0x18C` (title ID), `0x198` (group ID), `0x1DC` (title version), `0x1DE` (content count), `0x1E0` (boot index), and 36-byte content records from `0x1E4`. Sections are 64-byte aligned. Nothing was decrypted.
- **Observations:**
  - Header size `0x20`, type `Is`, certificate chain 2,560 bytes, ticket 676 bytes, TMD 736 bytes, data 26,916,864 bytes, footer 409,344 bytes.
  - Ticket and TMD title ID `00010001-57505345`, ASCII `WPSE`. Group ID `0x3031` (ASCII `01`).
  - TMD system version `00000001-00000038` (IOS56). Title version 1. Seven contents. Boot index 6.

| Index | Content ID | Type | Size (bytes) |
| --- | --- | --- | --- |
| 0 | `00000000` | `0x0001` | 409,344 |
| 1 | `00000001` | `0x0001` | 2,004,320 |
| 2 | `00000002` | `0x8001` | 2,156,800 |
| 3 | `00000003` | `0x8001` | 370,880 |
| 4 | `00000004` | `0x8001` | 112,448 |
| 5 | `00000005` | `0x0001` | 21,550,432 |
| 6 | `00000006` | `0x0001` | 312,576 |

- **Interpretation (hypotheses):** the `E` in `WPSE` and the `01` group ID are consistent with a North American Nintendo-published title, matching the filename hint, but the region is not confirmed. The footer size equals content 0's size; whether this is meaningful is unknown. Content 0 is commonly the banner in WiiWare titles, and the boot content commonly a loader that starts a main executable stored elsewhere. Content type `0x8001` has not been interpreted. Content roles require decryption and inspection.
- **Confidence:** high for the raw field values; low for every interpretation.
- **Next validation:** extract contents with a recorded tool (planned: decomp-toolkit `dtk wad`/`dtk vfs`), confirm each content's format, document provenance, and fingerprint a provenance manifest. See the [roadmap](../ROADMAP.md#phase-1-extract-and-identify).
