# Artifact and section inventory

No target is selected yet. Inventory completeness is unknown. Only unencrypted WAD metadata has been read; no content has been decrypted or extracted.

## Observed container

The local WAD's header, ticket, and TMD are recorded in [F-0001](FINDINGS.md#f-0001-wad-container-metadata): title ID `WPSE`, title version 1, seven contents (indices 0 to 6), boot index 6. Content roles, formats, nested files, and acceptance-manifest membership are all unknown until extraction.

## Artifact records

For each discovered artifact record target, relative path, kind, size, SHA-256, parent container, extraction procedure, role, acceptance-manifest membership, and reconstruction strategy. Explicitly list excluded or unexamined content and the reason. Distinguish a preserved asset from an original executable dependency.

## Section records

For each executable record format, entry point, sections, file offsets, load addresses, sizes, alignment, permissions, zero-initialized regions, relocation behavior, and dependencies where observed. Mark unavailable information unknown. Link each record to the evidence in `FINDINGS.md`.

## Coverage review

Reconcile every container entry and executable module against this inventory before claiming full coverage. Establish the total executable code size and function-count method before reporting percentages. Review package metadata and final container reproduction separately from payload identity.
