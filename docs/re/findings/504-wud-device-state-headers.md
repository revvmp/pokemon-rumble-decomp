# WUD device-state header prerequisite

## F-504-5: Evidence-backed device member names

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. This prerequisite changes only two member identifiers in the existing `WUDDevInfo` declaration. It preserves their types, order and storage, along with the complete 0x60-byte structure. It adds no function body, split or matching credit. The source reconstruction remains in #719.

| Offset | Type | Descriptive name | Evidence and confidence |
| --- | --- | --- | --- |
| `0x5A` | `s8` | `pinCodeSubmitted` | The security callback stores 1 in the discovery-work record immediately before constructing and submitting the six-byte reversed-address PIN through `BTA_DmPinReply`. Behavior-based name, medium confidence in intent; original spelling is unknown. |
| `0x5C` | `u8` | `linkKeyStorage` | Stored-link-key results assign 1 for a device absent from NAND, and change a prior NAND-only value 2 to 3 after receiving the controller key. Initialization and cleanup reconcile NAND and controller records. High confidence in storage-source state; original spelling is unknown. |

The original security callback at `0x8030F180` derives the working record from `_wcb + 0x858`, equal to `_work` at `0x8047CB78`. The store at `0x8030F2F0` writes 1 to offset `0x5A`; `BTA_DmPinReply` is called at `0x8030F33C` with acceptance true and a six-byte PIN. This proves the flag's location and immediate use context, not an unevidenced consumer or reset protocol.

The original stored-key callback at `0x8030FBF0` stores value 1 at offset `0x5C` at `0x8030FDD0` when allocating a record for a controller key not found in NAND. It then copies the six-byte address and sixteen-byte key. Its existing-record path compares the byte to 2 at `0x8030FE20` and stores 3 at `0x8030FE28`. Initialization stores 2 after loading NAND information, while cleanup deletes controller-only keys and clears their state. These accesses establish the reconciliation interpretation: 0 cleared, 1 controller only, 2 NAND only, and 3 present in both. This byte is separate from connection `status` at `0x59`.

The pinned [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found, supplies the corresponding functional contexts. [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), also CC0, corroborates both roles but retains offset-based names itself. The new names are descriptive reconstruction names, not purported recovered Nintendo identifiers.

## Validation

The header is compiled through the actual `all_source` entry point before and after the rename. SHA-256 comparisons show all 358 pre-existing compiled source objects are byte-identical after the rename on main `beeb2d31`. A full Ninja build and explicit frozen executable verification check the final DOL. The final evidence records the object count and verified commit in the pull request description. No compiler flag, original byte, expected digest, member type, offset or structure extent changes.

## Traps

- **Link-key reconciliation state is not connection state.** Values 1/2/3 at offset `0x5C` track which storage sources contain a device's key. Connection status is the separate byte at `0x59`; keep those roles distinct when reconstructing the synchronization and stored-key callbacks.
