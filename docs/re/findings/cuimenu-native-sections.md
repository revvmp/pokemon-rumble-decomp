# CUIMenu native source-section ordering

## Remaining emission difference

The coordinator's `compare_request` candidate reproduced all 61 real functions and the retained data and exception records. A full source-selected link still differed in 186 bytes because `NotifyCallbacks` and `vector<shared_ptr<IUIListItem> >::iterator::operator!=` appeared in the opposite order. The difference included their addresses, branch operands, and exception-index references. The preceding cleanup identity correction belongs to the coordinator's CUIList/CUIMenu evidence.

The ordinary declaration/request experiments did not remove this final swap. The accepted section candidate preserves the same C++ bodies and uses the compiler's ordinary `__declspec(section)` facility to group the first 26 functions, `NotifyCallbacks`, and the remaining functions. The native final linker places these three input sections in that order inside the executable's existing `.text` output. No object bytes, relocation records, original inputs, or expected hashes are rewritten.

## Source controls

The source declares `.text.menu_prefix` and `.text.menu_notify`; the remaining code uses `.text`. Four existing inline accessor definitions receive a section attribute only under `CUIMENU_EMISSION_LAYOUT`: `PaneRectangle::GetWidth`, `CUIMenuItem::GetKind`, and the two `CUIList::GetItem` overloads. The coordinator's existing shared-pointer default-constructor view remains scoped to the same macro.

Late explicit specialization declarations place the two pointer accessors, vector constructor, and vector reserve function in the prefix. The CUIMenuItem shared-pointer boolean conversion needs an early inline specialization containing its unchanged original template body: a late section declaration is accepted but does not move this conversion. The implicit CUIMenuItem control-block destructor otherwise inherits the prefix section from its class instantiation, so a late typed declaration explicitly restores it to `.text`.

These controls, the existing unused comparison request, and the default-constructor emission view are D-0016 source-organization accommodations. They do not establish the original developers' source organization. The early boolean-conversion specialization copies the existing reconstructed generic C++ implementation, not executable bytes.

## Validation

The coordinator's native `menu-sections` trial selected the genuine compiled candidate and passed the frozen target verifier with zero differing DOL bytes. The accepted executable SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scratch evidence is under `tools/local/tail-20261104/links/menu-sections`; the first candidate object is `game_CUIMenu/sections_complete/CUIMenu.o` in the same scratch root.

The report-compatible candidate renames the custom sections to the `.text.` family. Its 61 target functions have unchanged raw instruction bytes compared with the coordinator's original candidate, and concatenating the three sections yields all 61 functions in target order. Its `.data`, `.sdata`, `.sdata2`, and `extab` bytes remain unchanged. `objdiff-cli diff -c combine_text_sections=true` reports 61/61 functions at 100 percent. This setting combines section families for analysis; it does not modify linker inputs. The upstream [objdiff section reader](https://github.com/encounter/objdiff/blob/main/objdiff-core/src/obj/read.rs) groups sections by the name before the last dot or dollar separator, so arbitrary `.menu_prefix` and `.menu_notify` names do not combine with `.text`.

The cleaned source and narrowly gated header proposal are under `game_CUIMenu/sections_clean` and `game_CUIMenu/section_clean_include`. Recompiling the cleaned source preserves every allocated section byte of the report-compatible candidate. The renamed `menu-report-sections` native trial also passed the frozen verifier.

The cleaned source and scoped header changes are now installed. `configure.py` marks CUIMenu Matching with the ordered `.text.menu_prefix`, `.text.menu_notify`, `.text` input sections. The normal build invokes `tools/build/order_text_sections.py`, then the pinned original linker and dtk conversion. The generated objdiff unit has `options.combine_text_sections=true`, so the standard `tools/re/unitdiff.py CUIMenu --no-build` reports 61/61 exact functions without extra flags.

The ordinary production ninja run exited 0; `build/wpse-v1/verification.json` passed the frozen manifest. A separate byte comparison confirmed all 1,385 aliased objects equal their selected source/fallback inputs. No formerly exact real function regressed. CUIMenu adds 11,812 source-linked code bytes and 3,772 data bytes, for totals at its acceptance of 2,696,220 code bytes and 1,660,427 data bytes. Remaining real function mismatches at that point: 30.

Two subsequent clean builds, including the six function gains and RGB565 source linkage, each regenerated the entire `build/wpse-v1` directory and passed the frozen verifier. Their `main.dol`, `main.elf` and `report.json` are byte-identical. See [24-function checkpoint validation](../../VALIDATION.md#24-function-checkpoint-and-native-section-linking). The preparation CLI also rejects invalid text-section selection and a template lacking exactly one empty `.text` output section; both actual CLI invocations exited 1 before producing a linker command file.

## Traps

- An accepted late section annotation can leave a conversion operator in its original section. Inspect the compiler's section membership; an early same-body inline specialization moved this conversion.
- A synthesized control-block destructor can inherit the section active when its class is instantiated. Audit all retained functions, not just the functions intentionally annotated.
- Objdiff only combines custom text sections sharing the `.text` base name. Enable `combine_text_sections` and use `.text.descriptive_name`; a native full-link SHA remains the acceptance evidence.
