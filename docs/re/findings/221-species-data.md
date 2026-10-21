# F-221-3: Species-record correction and native accessor checkpoint

The following unit begins at `0x80124C80` with species validation and ends at `0x80125180`, after the special-form model-number mapping. The boundary includes twenty-one functions, crossing into #222. Ten functions and 824 instruction bytes belong to the remaining original #221 inventory. The controller-table unit was accepted separately in PR #1079 (`ca7d10184a`); #221 remains incomplete.

## Corrected complete record contract

The declaration-only `PiiSpeciesData` introduced with F-115-6 has the correct 28-byte extent, but two inferred field interpretations are disproved by direct native evidence. Offsets `0xC` and `0x10` contain model-name pointers, not move IDs. The first record points twice to the string `HUSIGIDANE` at `0x803B8730`; the Venusaur record points to `HUSIGIBANA_M` and `HUSIGIBANA_F` at `0x803B8748` and `0x803B8758`. Function `0x80125080` returns one of these pointers indexed by `sex & 1`.

The two floats at `0x14` and `0x18` are walk-speed coefficient and walk-animation rate, rather than model/shadow scale. Native script wrappers `method_pprGetWalkSpeedCoeff` (`0x80158250`) and `method_pprGetWalkAnmRate` (`0x801582D0`) call precisely the corresponding accessors at `0x80125090` and `0x801250A0`. The original binding table establishes those meanings independently of the record values.

Other native script wrappers establish the packed fields' meanings: `pprGetBoneType` calls `0x80124F30`, `pprGetFlyHeight` calls `0x80124F40`, and `pprGetASize` calls `0x80124F50`. `pprGetSexVector` calls `0x80125010`. The original wrapper names are recorded in `docs/re/script-bindings.tsv`. The class name and accessor spellings remain descriptive; embedded binding names support the member meanings.

## Private scratch producer evidence

A complete ordinary C++ scratch implementation compiles with canonical Wii/1.1 game options. Twenty of the twenty-one function instruction streams reproduce native sizes and every non-relocation instruction word. This is diagnostic evidence, not matching acceptance: relocation targets, retained exception/data intervals and the actual source-only link remain to be accepted. The remaining form validator is the native size, 572 bytes; exactly thirteen switch-tree comparisons use signed `cmpwi` rather than the native unsigned `cmplwi`. Wii/1.0 and Wii/1.3 reproduce the same residual and do not justify changing the canonical compiler.

The packed layout uses a 16-bit nine-bit species field followed by an eight-bit five-bit form field in the partially occupied second byte. Using `u16` for both fields produces an incorrect halfword load in the form getter; the mixed native storage units reproduce both getters exactly. The remaining fields use the observed halfword/byte bitfields, and the two model pointers and floats keep the overall 28-byte layout. The complete one-element canonical empty-text declaration permits native SDA addressing in the name getter; an incomplete array declaration adds an address-load instruction.

The record and form-offset tables are referenced at `0x8035DF88` and `0x80361CC8`. Their source-unit ownership is still under investigation. The model-name string pool begins after the accepted ScriptCamera data range, far after this accessor unit's small combined-type-effectiveness table at `0x803B3070`; address separation alone does not establish a source boundary. No table fallback is being claimed as reconstructed source, and no executable-byte array is introduced.

## Traps

A complete, consumer-matching declaration can still contain incorrect inferred member meanings. Verify stored pointer values and the actual accessor return contract before reusing a field name. Here the native sex-indexed strings disprove the earlier move-ID interpretation, and independently named script wrappers disprove both float interpretations. Canonical declaration changes require current-consumer and whole-artifact controls before publication; scratch instruction agreement does not replace those gates.

## F-221-4: Bounded canonical declaration correction

The canonical change is limited to the two model-name pointers and the two proven float names. Existing packed-word storage, all public signatures, record size/alignment and the surrounding consumers are preserved. The private bitfield/accessor scratch is not published as accepted code.

On base `ca7d10184aed13eea0dc24ba044113ccdaeedfe3`, the baseline and candidate each compile all 722 actual Ninja `all_source` objects, execute the default build after removing only the old DOL, and pass explicit frozen-manifest verification. Every complete source object is raw-identical. The complete linked ELF is raw-identical, SHA-256 `6a74860a8eed177ab59aeb007b278152a51a637108e4de3344d2dcee24fd9ed3`; the DOL remains the frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The same control is rerun inside the official landing verifier after each rebase. This corrects the declaration's meaning and pointer types without adding source coverage or completing #221.
