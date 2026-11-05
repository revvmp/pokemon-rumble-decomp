# NW4R object data boundary audit

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Scope and baseline

The 2026-11-05 audit reads the existing target/source objects and the real objdiff report. The overall report measures 1,245,840 matched data bytes out of 1,788,360, or 69.663826%, separately from its 100% source-selection measure. The NW4R path inventory contains 195 logical units, 226,043 data bytes and 168,211 matched data bytes; 53 units have incomplete object-level data credit.

The main executable's prior exact-link acceptance does not make the object-level Matched Data field 100%. This audit preserves that distinction and does not modify originals, frozen hashes, source implementations, compiled objects or section ranges.

## EffectSystem destructor-registration boundary

The target object gives `mSortEffectOperatorZDrawInfo` the interval `0x80423190`–`0x8042323C`, size `0xAC`. The compiled `DrawInfo` object is actually `0xA0` bytes. A separate compiler-generated destructor-registration record occupies the following twelve bytes, at `0x80423230`. The `EffectSystem::instance` object starts at `0x8042323C` in both objects and has the same `0xC06C` size.

This interpretation is independently visible in the original static initializer. Relative to its `.text` start, instruction `0x978` computes `r5 = r29 + 0xA0`; instruction `0x98C` calls `__register_global_object`. Here `r29` is the BSS base `0x80423190`, `r3` is the EffectSystem instance at `r29 + 0xAC`, and `r4` is its destructor. The final DrawInfo initialization store is at member offset `0x9F`.

A private copied target object corrected the DrawInfo symbol size and added the independently proven twelve-byte registration symbol. Its `.bss` section remained exactly `0xC118` bytes. Comparing it against the untouched compiled source object changed the BSS comparison from 99.82603% to 100%, recovering 49,432 bytes of section-level credit in the diagnostic. The `.data` comparison remained 85.71429% because the source object also emits a dead weak DrawOrderBase vtable.

The production correction is to give DrawInfo size `0xA0` and identify `sEffectSystemDestructorRegistration` at `.bss:0x80423230`, size `0xC`. No source padding or additional data definition is needed.

## Other zero-initialized symbol extents

Private copied-object experiments also corrected the following inflated symbol sizes without changing their addresses, bytes or containing section extents. Each affected BSS/SBSS section then compared at 100% against its existing compiler object.

| Owner and symbol | Old size | Proven size | Section credit recovered |
| --- | ---: | ---: | ---: |
| AxManager registration record at `0x8044D280` | `0x10` | `0xC` | 1,040 |
| ChannelManager registration record at `0x8044D3C8` | `0x10` | `0xC` | 48 |
| AxVoiceManager instance guard | 8 | 1 | 8 |
| ChannelManager instance guard | 8 | 1 | 8 |
| DisposeCallbackManager instance guard | 8 | 1 | 8 |
| RemoteSpeakerManager instance guard | 8 | 1 | 8 |
| BasicSound typeInfo | 8 | 4 | 8 |
| IOStream typeInfo | 8 | 4 | 8 |
| FileStream typeInfo | 8 | 4 | 8 |
| DvdFileStream typeInfo | 8 | 4 | 8 |
| NandFileStream typeInfo | 8 | 4 | 8 |
| Sound3DManager default engine | `0x18` | `0x14` | 24 |
| Wide TextWriterBase default-tag-processor guard at `0x804B93F1` | 7 | 1 | 24 |

The guard, typeInfo and default-engine identities already exist in source and compiler symbol tables. The registration records have the runtime's twelve-byte structure; their target labels had absorbed following alignment padding. The private machine-readable proposal is `tools/local/nw4r-data-audit/symbol-corrections.json`, with fourteen symbol-size updates and one added EffectSystem registration symbol. These diagnostic corrections account for 50,640 bytes of NW4R section credit. The coordinator must regenerate targets with ordinary dtk and verify the complete frozen DOL before accepting production results.

## Remaining data differences

The BiquadFilterPreset coefficient arrays already agree in content. Its target `.rodata` includes two trailing zero bytes that the compiled section omits, so the 5,176-byte section initially scores 99.980675% and contributes no matched-data credit. Its `.sdata2` similarly includes four trailing zero bytes after the compiler's twelve bytes. Three coefficient symbols absorb two bytes of alignment: Hpf has actual size `0x3CA`, and Bpf1024/Bpf2048 each have size `0x3A2`. Their target sizes were respectively `0x3CC`, `0x3A4` and `0x3A4`. The final sdata2 literal is a four-byte float, not an eight-byte object. Correcting these four symbol sizes alone makes both complete sections score 100%, preserving the 5,176-byte and sixteen-byte section extents. The comparator recognizes their remaining bytes as alignment padding once the object boundaries are correct.

The same private-copy experiment recovers many other initialized-data sections without altering bytes or section boundaries. Vtable tails, floating constants, RTTI name records and the 33-byte Lfo sine table had absorbed padding. The full proposal with before/after sizes and measured section scores is `tools/local/nw4r-data-audit/initialized-symbol-corrections.json`. Its two font-record corrections improve their sections but need additional identity mapping before reaching 100%.

Three StrmPlayer vtables at `0x803C67D0`, `0x803C6818` and `0x803C6830` match the source's StrmPlayer, StrmDataLoadTask and StrmHeaderLoadTask objects. Restoring those identities makes the 120-byte data section score 100%. AnmObjVisRes's type name has size `0x14`, and SeqPlayer's vtable has size `0x64`; correcting their previously padded extents restores their 40-byte rodata and 104-byte data sections to 100%. Those proposals are recorded separately in `tools/local/nw4r-data-audit/identity-symbol-corrections.json`.

Seven further data groups reach 100% with explicit target-to-compiler symbol mappings: EffectSystem's version banner, ParticleManager's switch table, ResAnmChr's literals, Camera's literals, ScnRoot's literal, and the ResFontBase/PairFont member-function-pointer records. The fonts also require their already demonstrated twelve-byte record extents. These mappings are recorded in `tools/local/nw4r-data-audit/mapping-corrections.json` and were exercised through a private ordinary objdiff project.

Camera illustrates why equal source/target offsets alone are insufficient. Its compiled object inserts an unused eight-byte literal before the final two live float constants. The original instructions use `lfs` at `0x804BACD0` and `0x804BACD4`; these are separate four-byte floats, not a split double. They correspond to compiled `@4415` and `@4416` at offsets `0x30` and `0x34`. Explicitly mapping all eleven original literals restores 100% while retaining the discarded source literal and every original section byte.

## CntFileStream exception identity diagnostic

The ordinary project mappings pair all nine original exception records and indexes with the correct compiler symbols. Each mapped exception body scores 100%. Six mapped indexes after the discarded IOStream destructor nevertheless score 80%, yielding 86.666664% for the 108-byte index section. The original and compiler indexes have equal byte contents and corresponding function and exception-record relocations.

A private target copy renamed only its existing exception/index symbol names to the corresponding generated compiler names from the same mapping. Symbol addresses, sizes, section contents and relocation records remained unchanged. A direct objdiff comparison then reports 100% for both exception sections. The private artifact is `tools/local/nw4r-data-audit/ut_CntFileStream-names.o`. This isolates the remaining difference to mapped relocation-identity handling in objdiff 3.8.2, rather than reconstructed exception bytes. The experiment is diagnostic only; it is not an accepted build-object transformation or a reason to modify source unwind behavior.

## Complete configured-unit review

After the coordinator integrated the proposed metadata and comparison mappings, an independent invocation of the ordinary `objdiff-cli diff -p . -u` command for each of all 195 NW4R-path units found only two remaining non-100% DATA/BSS sections: CntFileStream's 108-byte exception index and StrmPlayer's eight-byte SBSS. All other initialized data, BSS, small data and small constants compared at 100% through the active project configuration. CntFileStream belongs to the game's progress category even though its source path is under NW4R, accounting for the category/path total distinction.

StrmPlayer's remaining symbol at `0x804B9390` is `bool StrmPlayer::sStaticInitFlag`. The original constructor loads it with `lbz` at unit text offset `0xA0` and stores it with `stb` at `0xBC`. The source declaration and compiler symbol both occupy one byte. Correcting its inferred eight-byte extent to one byte and restoring the canonical `sStaticInitFlag__Q44nw4r3snd6detail10StrmPlayer` identity makes the unchanged eight-byte SBSS section score 100% in a private object comparison. No source change is required.

Many smaller NW4R differences likewise arise from trailing zero padding, while other objects retain extra compiler-emitted weak metadata that the final linker discards. Neither class of difference justifies altering the original executable or inventing padding members in otherwise correct types. A separate diagnostic inventory is stored at `tools/local/nw4r-data-audit/data-diffs.json`; insertion entries without serialized data do not prove their content is zero.

## Traps

An exact final executable can still have incomplete pre-link data scoring because inferred target symbols absorb compiler-registration records or alignment padding. Inspect the original initializer's accesses, real source types and compiled symbol extents before changing source storage. Correct only independently demonstrated target metadata; retain full section coverage and require the normal full-link verifier afterward.

The generated report can omit a section's zero-valued fuzzy-match field. Treat a missing field as zero when filtering unmatched sections; defaulting it to 100% hid StrmPlayer's SBSS in the initial section-level scan, although the unit's total matched-data deficit remained visible. The final complete configured-unit review checks both DATA and BSS kinds with a zero default.
