# WUD standard pairing and Wii Fit record

## F-504-9: Standard-device registration and typed pairing data

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. The reconstructed `__wudSyncVirginStandard` at `0x8030A3A0` has its exact original 0x378-byte extent. The name follows the pinned key reference with medium original-spelling confidence. Every non-relocation instruction word is identical except four folded literal addresses. Each differing instruction has identical opcode/register operands and addresses byte-identical device-name or entry-index message text.

The routine preserves a registered Balance Board when allocating a full standard-device table for a Wii Remote, removes the appropriate oldest entry, obtains an unused record, copies the 0x60-byte working device, registers it and moves it to the front of the standard list. For a linked Balance Board, it also updates settings slot 14 and the Wii Fit pairing workspace before recording the device-handle address. It returns the original stored-link-key state 7.

## Private layouts

The new file-private SDK format declarations live in `include/private/wud/WUDWiiFit.h`. No existing shared type changes. Both names are descriptive reconstruction names, not recovered original identifiers.

| `WUDWiiFitConfigProfile` member | Offset | Bytes | Evidence |
| --- | --- | --- | --- |
| `devName` | `0x0` | `20` | Existing copied profile prefix |
| `linkKey` | `0x14` | `16` | Target copies the device key into this exact settings-profile position |
| `profileData` | `0x24` | `28` | Remaining copied, uninterpreted profile bytes |

This view is 64 bytes and applies only to the byte-array profile in settings slot 14. Its key position differs from the existing generic `SCDevInfo` declaration, so the source uses a private format view instead of changing the shared ABI.

| `WUDWiiFitPairingInfo` member | Offset | Bytes | Evidence |
| --- | --- | --- | --- |
| `devAddr` | `0x0` | `6` | Optional address copy |
| `linkKey` | `0x6` | `16` | Optional key copy |
| `devInfo` | `0x16` | `64` | Full existing `SCDevInfo` copy |
| `reservedData` | `0x56` | `42` | Uninterpreted bytes retained in the checksum input |
| `checksum` | `0x80` | `4` | Target's stored checksum word |
| `alignmentPadding` | `0x84` | `28` | Remaining workspace within the reference-supported 0xA0-byte extent |

The workspace at `0x8047D040` is 160 bytes with 32-byte alignment. Its active checksum input is the first 128 bytes, followed by the four-byte checksum; this routine does not interpret the last 28 workspace bytes. The pinned key reference also gives this workspace a 0xA0-byte extent. Its following handle array starts at `0x8047D0E0`. The source's typed record naturally reproduces both BSS positions, extending the exact prefix established by F-504-8. No executable byte array or fabricated alignment object is used.

The checksum helper keeps the reference name `__wudCalcWiiFitCrc2`, but its proven algorithm is a seeded additive checksum over 64 halfwords: the upper half accumulates each word, and the lower half accumulates each complemented word with 16-bit wrapping. The target's complete inlined checksum instruction stream matches. Paired SDA operands identify four-byte `sWudWiiFitChecksumSeed` at `0x804B9B44`. Names for the workspace and seed are descriptive; address and storage confidence are high.

The call to `WUDiMoveTopOfUnusedStdDevice` names the already-owned routine at `0x8030EF20`; its body remains unreconstructed. Its spelling follows the pinned reference with medium confidence. Its type and extent remain unchanged.

## Source and validation

The allocator helper initializes its null candidate only inside the standard-device branch, as in the newer reference's split standard allocator. This reproduces the target's initialization placement. `__wudGetNewDevInfo` is explicitly declared inline to express the observed call-site inlining; the original spelling of that qualifier is not claimed. Four other added functional helpers inline automatically. The board lookup uses a direct pointer truth test, preserving the newer reference's behavior and the original branch sequence. None of these five helpers has a standalone source-object body.

At this standard-pairing pass, the actual `all_source` target compiled 50 retained native functions totaling 9,356 function bytes. The raw audit still proves 43 functions / 5,192 exact non-relocation bytes. Seven exact-size partial bodies retain 22/4/8/7/5/5/4 folded operands for security/registration/removal/search/power/connection attempts/standard pairing. All 55 operands resolve to byte-identical data. The descriptor and reset command retain 245 exact initialized bytes. Whole-unit diagnostics are text 37.301422%, data 5.473637%, BSS 66.66667%, sdata 0%, sbss 45.28302% and sdata2 100%.

Direct comparisons verify 50 function addresses/extents, both newly named bindings, the pairing workspace and handle-array BSS positions, all partial data operands and helper retention. All 340 pre-existing main-branch source objects remain byte-identical. Full Ninja exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`.

WUD remains `NonMatching` and the original object remains selected for the final link, adding zero linked WUD source credit. Complete the remaining 36 functions, complete data layout, folded operands and all relocations before acceptance.

## Provenance

The standard-pairing and Wii Fit paths follow [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Allocator/count helpers and retained entry-index logging follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. The source replaces offset casts with the proven private layouts. Comments and annotations are removed; no decompiler output is included.

## Traps

- **A settings slot can have its own profile format.** This Wii Fit slot writes its key at profile offset 0x14, unlike the generic shared `SCDevInfo` key member. Keep the format-specific view private and preserve all other copied bytes.
- **Branch-local initialization affects scheduling.** Initializing the allocator candidate before disabling interrupts moved the null assignment ahead of the target sequence. Initialize it only in the standard-device branch, which actually needs the null-search result; the other branch assigns the list-tail record directly.
