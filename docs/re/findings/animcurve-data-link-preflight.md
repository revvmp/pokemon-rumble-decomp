# Animation-curve retained data and link layout

## Scope

This is a partial source-link audit, not acceptance of the unfinished animation-curve unit. The private combined snapshot uses `selector_u8_record_1` for U8, `animcurve_f32_scale_priority_31` for F32, and `rotate_float_access_1` for Rotate. Only each helper/function fragment is copied into the canonical source; the already exact Child, Texture, CreateCurveChild, and baked functions are preserved. U8 and Rotate have identical `HashCurveExperiment` definitions, so the combined source keeps that definition once. All three diagnostic scores are unchanged by combination: 99.486755, 99.52028, and 99.8423 respectively. The other five functions remain exact.

The packet is `tools/local/tail-20261104/nw4r_ef_ef_animcurve/pf_animcurve_combined_data/`. The direct compiler uses the configured Wii/1.1 flags without changing the shared build. The frozen original object is `animcurve-original.o` in the same unit directory, SHA-256 `214d84b4511c8f10d599c044bd912044f85afc5800d11630d30f2ce1c07e24d2`. The script `pf_animcurve_data.py` reconstructs the combined snapshot and writes `retained-data-audit.json`.

## Tables and literals

The complete 512-byte `.data` section and 56-byte `.sdata2` section are byte-identical to the original split object and the frozen `orig/wpse-v1/main.dol`. Their section types, flags, sizes, and eight-byte alignment agree. Neither section has relocation entries. The DOL hash is the frozen `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The two byte tables are `sChannelCount` at `0x803C5378` and `sHighestChannel` at `0x803C5478`, each 256 bytes. They preserve the population count and highest selected channel bit for every input byte, including zero. The literal pool begins at `0x804BA628`:

| Offset | Representation | Value or purpose |
| --- | --- | --- |
| `0x00` | float | 0.0 |
| `0x04` | four zero bytes | alignment padding |
| `0x08` | double `4330000000000000` | unsigned conversion bias |
| `0x10` | double `4330000080000000` | signed conversion bias |
| `0x18` | float | 1.0 |
| `0x1C` | float | 2^-23 |
| `0x20` | float | 65536.0 |
| `0x24` | float | 3.0 |
| `0x28` | float | 2.0 |
| `0x2C` | float | 1.5 |
| `0x30` | float | -2.0 |
| `0x34` | float | 255.0 |

All eight original functions have the same relocation offsets, kinds, and destinations. The comparison resolves unit-local data references to section offsets, because the split object's named table symbols have global binding while the source's static tables have local binding. Whole-section byte comparison independently proves the referred-to values. The audited unit has no exception sections under its configured exceptions-off flags.

## Retained function layout

All eight function extents and their relative order agree with the original. Original inter-function padding is reproduced, including the four, eight, and twelve-byte gaps. The source object contains two pre-existing extra functions, AnimCurveCalcTime and AnimCurveCalcFrameStep, before the retained unit. They occupy 1,520 bytes including their padding. These functions have no original symbol identities in the configured executable and are discarded by the native link.

The original split `.text` section has size 20,236 and alignment four; the compiler's complete `.text` has size 21,756 and alignment sixteen. These section-level differences do not themselves establish a link mismatch: the native linker selects retained functions and their observed alignment. The diagnostic link below confirms their resulting layout.

## Native diagnostic link

The coordinator linked the combined source object through the real native linker and converted the ELF to DOL successfully. `tools/local/tail-20261104/links/animcurve-layout-diagnostic/verification.json` correctly reports `FAIL`, because three function bodies remain unfinished. The output has the original 4,094,880-byte size. Its SHA-256 is `0a4baa7a73f1bcc0c68e0cc8c4b0373491690086bf7cfe7536f9ba4992579dd5`.

An exhaustive byte comparison classifies all 263 differing bytes:

| Original function | Original range | Differing bytes |
| --- | --- | --- |
| AnimCurveExecuteU8 | `0x802290A0` through `0x8022A37F` | 153 |
| AnimCurveExecuteF32 | `0x8022A380` through `0x8022B34B` | 71 |
| AnimCurveExecuteRotate | `0x8022B350` through `0x8022C427` | 39 |

There are zero differences outside those three bodies. DOL headers, every other function, padding, data, and metadata remain byte-identical. `difference-scope.json` records every differing byte and its owner; `pf_animcurve_link_audit.py` reproduces that classification.

This snapshot has no additional data, literal, emission-order, or link-layout obstacle. Later function candidates still need their own final native source-link verification. In particular, the subsequent Rotate `rotate_flag_load_shapes_0_1` gain was reported after this diagnostic snapshot and is not included in its hash or byte counts. No canonical source, header, original hash, or build configuration was changed by this preflight.

## Final source acceptance

After Rotate and F32 acceptance, the coordinator repeated the native diagnostic with the final-size U8 candidate. All 36 differing bytes lay in 24 U8 instruction words; none lay outside that function. The final U8 candidate then removes those differences completely. The native replacement links, converts and passes frozen verification with zero differing DOL bytes. The ordinary build now selects the animation source object as Matching and passes the same verification. All eight functions, their relocation-backed literals and both retained data sections are exact. See [the final source evidence](animcurve-20261104-continuation.md) and [validation](../../VALIDATION.md#two-function-checkpoint-and-animation-source-acceptance).
