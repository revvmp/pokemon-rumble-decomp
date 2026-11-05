# Semantic reconstruction of the embedded system-message archive

The 11,488-byte `gSystemMessageArchive` can be rebuilt exactly from an editable layout, three editable animations and two PNG images. The build inputs contain named records, text, colors, transforms and keyframes. They contain no stored archive offsets, opaque block payloads, executable instructions or slices of the original DOL.

This is asset reconstruction. It does not reconstruct any additional game function or the separate DSP program. The original authoring files were not available; the editable records and images were recovered from the frozen executable. The coordinator owns normal build integration and final accounting.

## Original artifact and ownership

The original is `.rodata:0x80363A80` through `0x80366760`, with SHA-256 `e27d380bbad3aff2ef2d41cfdc5b77590390a68937fef6edc87502f2d1813161`. It belongs to the frozen main DOL whose SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`, recorded in [the target manifest](../../../config/targets/wpse-v1.json). Neither original file nor manifest was modified.

[Main initialization](../../../src/game/main.cpp) passes the archive to `InitializeSystemMessages` with layout name `fatal_errmsg` and external font `fot_wanpaku_b.brfnt`. [MainRuntime.h](../../../include/game/MainRuntime.h) declares the existing const unsigned-char archive symbol. The archive has six files and eleven U8 nodes:

| Member below `arc/` | Bytes | SHA-256 |
| --- | ---: | --- |
| `anim/fatal_errmsg_begin.brlan` | 368 | `d2ac29cfd36aeca1248570c69982e8619a72c778b6c9cdcd694a0ab935d1d928` |
| `anim/fatal_errmsg_button_a.brlan` | 384 | `6155fa9a07e3ab947ff9e3fd41aea8a3e893468a01c96f0ed3aafa38539598df` |
| `anim/fatal_errmsg_end.brlan` | 364 | `06fb588702ddec03cf4ea39abd73d4f7a11d288266c8fbaa7e59f9f4cc0796da` |
| `blyt/fatal_errmsg.brlyt` | 7,688 | `4770adc19851a172df8fa92dab2a7746e242589a36bffea65502695eebffe3e3` |
| `timg/button_a.tpl` | 2,112 | `c4ad85cfe63e887ab4f7c20ca70578b18fec3318f056d29d689ba43f4ee5b3bf` |
| `timg/window.tpl` | 192 | `2c737692f3d072e0f83bc0b0e6a3b83b8ffea005ca8b4bff5894f05e66985257` |

None of these member hashes appears in the 3,420-file recursive extraction inventory. The first container experiment reused original member payloads and reproduced the U8 archive, but that experiment alone was not reconstruction. The accepted candidate instead generates each member independently from the semantic inputs below.

## Recovered editable inputs

The accepted tracked asset directory is `orig/wpse-v1/reconstructed/system_message/`. This keeps proprietary recovered assets under `orig/`, as required by [project rules](../../PROJECT_RULES.md), without changing any frozen reference artifact. The private ready packet has that exact directory structure under `tools/local/tail-20261104/vm_system_asset_packet/`.

| Input | Editable content |
| --- | --- |
| `manifest.json` | Archive member names and their layout, animation or texture source paths |
| `layout/fatal_errmsg.json` | Thirty-five named panes, thirty materials, font and texture references, text and hierarchy |
| `animation/fatal_errmsg_begin.json` | Window/button scale and root alpha keyframes for opening |
| `animation/fatal_errmsg_button_a.json` | Window/button scale and root alpha keyframes for the button animation |
| `animation/fatal_errmsg_end.json` | Window/button scale and root alpha keyframes for closing |
| `texture/button_a.json` and `button_a.png` | Texture settings and a 64 by 64 grayscale image |
| `texture/window.json` and `window.png` | Texture settings and a 16 by 16 grayscale image |

`provenance.json` additionally records the original DOL identity, archive symbol/range, member hashes and recovered input hashes. It is documentation, not a build input. The original asset ownership is unchanged; placing recovered files under the approved directory does not relicense them.

The layout follows the existing [NW4R resource structures](../../../include/nw4r/lyt/lyt_resources.h), [pane implementation](../../../include/nw4r/lyt/lyt_pane.h), [position types](../../../include/nw4r/lyt/lyt_types.h) and [picture format](../../../include/nw4r/lyt/lyt_picture.h). Pane visibility, inherited alpha and location adjustment use separate boolean fields. Origins use horizontal and vertical names. Material, texture and font references use their names rather than stored indexes. Unicode text is encoded as UTF-16BE with a validated capacity. The thirty materials use the observed color tables, texture maps, transforms and texture-coordinate generators; unsupported resource forms are not copied as raw bytes.

The animations follow [NW4R animation execution](../../../src/nw4r/lyt/lyt_animation.cpp). Their targets are named `scale_x`, `scale_y` and `alpha`; each key specifies frame, value and slope. Content names refer to `window`, `button` and `root`. The encoder calculates tag, content, track and key offsets. Empty animation-group lists still point to the calculated end of the aligned tag name. JSON floats use short decimal representations that preserve the original encoded float32 bits.

Both textures are unpaletted `GX_TF_I4`. Their pixels are recovered to 8-bit grayscale PNG values, with each four-bit intensity represented by its value multiplied by seventeen. The encoder reconstructs the eight-by-eight tiled, high-nibble-first pixel order and complete TPL metadata. All PNG filters are decoded; supported inputs are non-interlaced 8-bit grayscale PNGs. Values outside the sixteen exact I4 levels are rejected rather than silently quantized.

U8 directory records, string tables and offsets are calculated from member paths. The builder sorts directory entries, aligns file payloads to thirty-two bytes and reconstructs the header's root offset, metadata size and payload start. No serialized U8 header or padding blob is an input.

## Build tool and end-to-end evidence

The ready packet contains five Python modules under `tools/assets/`: `build_system_message.py`, `nw4r_layout.py`, `gx_texture.py`, `u8_archive.py` and `system_message_schema.py`. They use the standard library. The packet contains encoders and the necessary PNG reader; the original BRLYT/BRLAN/TPL extraction decoders remain private diagnostic tools. All five modules have zero comments, docstrings and assertions. Runtime validation therefore remains active under `python -O`.

The entry point accepts a manifest and output archive, plus optional `--members` and `--c-include` destinations. The latter emits a comma-separated initializer for an ordinary const data wrapper. This generated initializer encodes the newly serialized assets; it is not checked-in original executable data.

`vm_system_assets_workflow.py` invokes the actual entry point with optimized Python. Its results are recorded in `tools/local/tail-20261104/vm_system_asset_workflow.json`; this is a private end-to-end diagnostic, not a unit-test suite.

- The positive isolated build reads exactly the nine JSON/PNG inputs listed above. A Python audit hook rejects undeclared workspace file access. No undeclared access occurs, and the builder never reads the original DOL, split objects, original members or provenance record. All six member hashes and the full archive hash agree with the original. The generated C initializer decodes to the same archive bytes.
- An edited-input build changes the first opening scale key from 1.05 to 1.1 and the first button-image intensity to seventeen. It succeeds, and decoding the output confirms the changed float32 value and pixel. Exactly the opening animation and button texture change; the other four members remain identical.
- Eleven rejection workflows cover an oversized pane name, duplicate material name, unknown material reference, insufficient UTF-16 text capacity, an unknown opaque-payload field, a non-finite key value, archive traversal, source traversal, a damaged PNG checksum, a non-I4 pixel value and duplicate JSON fields. Each exits 2 before emitting archive or include outputs.
- Three further rejection workflows direct the archive over its manifest, the include over a PNG and a member output over a JSON input. Each exits 2. Hashes of every source file are unchanged afterward. Resolved input/output paths are compared before any write.

The schema also validates numeric ranges, finite float32 encoding, named-resource and pane uniqueness, resource references, array dimensions, animation key order and supported material/texture forms. The scope is the observed asset formats, not a general-purpose NW4R authoring suite.

## Native link evidence

The coordinator compiled a normal Wii/1.1 C++ const unsigned-char wrapper from the semantic output. Its `.rodata` is exactly 11,488 bytes with alignment eight. The private native replacement at `tools/local/tail-20261104/links/system-message-semantic` links successfully and passes the unchanged frozen DOL SHA-256 with zero differing bytes. This verifies the archive's symbol placement and retention as well as its local content.

The coordinator then installed the tools, semantic assets, [source wrapper](../../../src/game/SystemMessageArchive.cpp), [header](../../../include/game/SystemMessageArchive.h), split and pre-compile build rule. The normal ninja build exits 0 after removal of the old output DOL, and frozen verification passes. The independent production audit reads the resulting archive, include, source object, linked ELF and main-link configuration:

- `build.ninja` lists exactly nine semantic inputs and five tool modules for the asset-generation edge. There are no missing or extra dependencies. The manifest is also a configure dependency.
- Ninja's compiler-dependency record is valid and contains the wrapper, header and generated `build/wpse-v1/include/system_message.inc`; the pre-compile order dependency is not the only link between generation and compilation.
- All nine installed input sizes and SHA-256 values agree with provenance. JSON files contain no CRLF pairs. `.gitattributes` gives reconstructed JSON `text eol=lf` and PNG `binary`, preserving those bytes across Windows checkouts.
- The generated archive, decoded C initializer, complete compiled `.rodata` and linked `gSystemMessageArchive` contents are byte-identical. The linked symbol remains at `0x80363A80`, size 11,488. The complete rebuilt DOL has the frozen SHA-256.
- `main_link.json` selects `src/game/SystemMessageArchive.o` among 900 raw native inputs. Only PiiPropData, ef_animcurve and the DSP image still use original objects. The source-linked data measure rises by the 11,488 reconstructed asset bytes; no CPU-code credit is added.

The production entry point emitted by configure is:

```text
python tools/assets/build_system_message.py orig/wpse-v1/reconstructed/system_message/manifest.json build/wpse-v1/assets/system_message.arc --c-include build/wpse-v1/include/system_message.inc
```

`tools/local/tail-20261104/vm_system_asset_production_audit.json` preserves the complete independent dependency/hash audit. The normal accepted checkpoint has 894 of 897 units source-linked and 1,777,877 of 1,788,360 linked data bytes; those figures include this explicitly identified asset reconstruction.

The DSP image remains an explicit executable-code dependency. Neither the PNG/TPL encoder nor the generated archive initializer contains DSP words. This archive reconstruction does not establish whole-release completion.

## Traps

- Repacking a container around original payloads proves only the container format. Here the U8-only proof was followed by independent layout, animation and image serialization before any asset-reconstruction claim.
- Generated output paths can overwrite their own editable inputs even when outputs do not overlap each other. Gather resolved manifest, JSON and PNG input paths, compare all output paths before writing, and exercise the real CLI with input hashes checked afterward.
- Assertions are insufficient for authoring-input validation because optimized Python removes them. Explicit exceptions keep the same rejection behavior under `python -O`.

## Repeated clean-build confirmation

Two complete clean target builds at b37f0dbb regenerate this archive and its C include from the installed semantic inputs. Both outputs reproduce their respective 11,488-byte and 68,928-byte hashes, and both complete executable builds pass frozen verification. The preceding incremental outputs are identical too. [Validation](../../VALIDATION.md#three-function-checkpoint) records all five compared artifacts.
