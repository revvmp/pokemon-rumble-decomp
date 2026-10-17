# F-812-2: Complete layout animation source unit

## Scope and provenance

The complete retained `nw4r/lyt/lyt_animation.cpp` unit covers all 27 functions / 5,396 instruction bytes in issue #812. It begins with GetStepCurveValue at `0x802AF0C0` and ends with the 132-byte transform-pointer UnbindAnimationLink overload at `0x802B05F0`–`0x802B0674`. The following ResourceAccessor destructor at `0x802B0680` belongs to the existing neighboring unit.

Source follows the CC0 Skyward Sword revision `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, raw source SHA-256 `c4a9c69ddea04a25075740d6c683e70a35d8a090996b3a4d3f6ea85e47075641`. [The interface prerequisite](812-animation-interfaces.md) records complete header provenance, layouts and owner coordination. The inspected source and headers have no separate file-level license exception. Comments and unused reference diagnostic stubs are not copied.

## Owned sections

| Section | Half-open native extent | Evidence |
| --- | --- | --- |
| `.text` | `0x802AF0C0`–`0x802B0674` | All 27 original inventory functions, including both anonymous curve evaluators |
| `.data` | `0x803C7228`–`0x803C724C` | Seven-entry AnimTransformBasic vtable, 36 bytes with eight-byte alignment |
| `.sdata2` | `0x804BB200`–`0x804BB220` | Complete eight-float constant pool, 32 bytes with eight-byte alignment |

The old vtable placeholder covered forty bytes, including four following alignment bytes. Only the owned symbol is renamed and narrowed to the proven 36-byte object; the next unit begins at `0x803C7250`. No neighboring split, size or type changes, and no original hashes change.

## Native diagnostic proof

The source compiles with established Wii/1.1 `cflags_nw4r`, default sixteen-byte function alignment, `-fp_contract off`, and `-ipa file`. All 27 native function sizes and every non-relocation instruction word are exact. The independent paired audit proves every one of the 67 retained text relocations and seven vtable pointers, plus all 68 initialized data bytes. All callees already have compatible evidenced names in the canonical symbol inventory; no outside symbol extent or type is changed.

Curve evaluation, resource allocation, both binding interfaces, pane/material animation, tag/share access, recursive animation counting, animation-tree construction/binding and both list-search overloads use ordinary C++ and the complete canonical class interfaces. The primitive Layout deletion helper reproduces the full native destructor without affecting neighboring Layout source ownership.

The retained release binding code performs no duplicate-link predicate. The reference's unused helpers return false before unreachable diagnostic code; those helpers and redundant conditions are omitted rather than introducing constant-false placeholder functions. The reference marks a final unretained resource-overload unbind body as guessed; it is omitted because it has no native target in this unit. The remaining ordinary unretained SDK helpers support the retained code and may be stripped by the linker.

## Integration state

The initial source branch temporarily reused only the #808/#811 owner's approved PR855 Material/TexMap declarations at `6b4489437a0ee0906d501e0a344fb846da663b98` and the proven unsigned-int offset signatures. After the actual canonical PR855 and PR857 merges, only the two owned source/split commits were replayed onto main `cfbc1e48b42ed8a4d2504dd137da2e4a432bd594`. All temporary header duplicates disappeared. The reviewed final diff contains only the owned source, metadata, ledger, symbols, findings and NOTICE; main's complete Material interface and coordinated headers remain unchanged.

The split-only baseline passed configure, all twelve updated default Ninja targets after split regeneration, and the explicit frozen verifier, all exit zero. This also verifies the temporary owner-approved prerequisite interfaces against the complete artifact before source registration. The unit was then registered NonMatching; actual source compilation, report and progress completed all four default Ninja targets, and frozen verification passed. The actual split/source objects compare at 100% in `.text`, `.data` and `.sdata2`.

After enabling the complete unit as Matching, the real source-selected ELF link, DOL conversion, SHA-1 check, report and progress completed all six updated default Ninja targets, exit zero. Explicit frozen verification and the independent final audit also exit zero. Every one of the original 27 functions has its native final ELF address and size, the vtable remains 36 bytes at `0x803C7228`, actual Ninja input selects only the source object and the complete rebuilt DOL is byte-identical to the frozen original. A separate paired audit on the actual compiled source object proves all 74 retained relocations and 68 initialized bytes again. The single ledger row credits exactly 5,396 original instruction bytes; unretained helpers and alignment receive no extra credit.

The canonical integration then passed a fresh 506-target plain Ninja build, explicit frozen verification, complete actual-object comparison and the actual-source paired relocation audit, all exit zero. All three allocated sections remain 100%; all original 27 functions retain their exact native final positions and sizes, with source-only input and the full DOL exact. No temporary header dependency remains. The latest-main ancestry guard and exact verified-head merge requirement still apply at the assigned integration slot. Header and source coverage remain separate throughout.

Source PR839 then consolidates this complete unit with the independently owned ScnMdl delivery after canonical PR845. A fresh 515-target default plain Ninja build and explicit frozen verifier pass; actual source-object, native final-address, source-only ELF-input and paired-relocation audits all pass again. All 27 functions / 5,396 instruction bytes, 74 retained relocation targets and 68 initialized bytes remain exact, and all three compared sections remain 100%. The combined gate also keeps all 506 pre-existing source objects whole-file byte-identical to actual main control. PR860 remains a preserved draft until the combined merge and issue closures are confirmed, then is superseded without duplicate source credit.

After the guards caught external PR878 and PR883, final integration repeats the fresh 515-target default build, frozen verifier and actual native/source-only audits on main `6965907b`. Every animation function, relocation, initialized byte and final native position remains exact. The earlier 506-object baseline result is historical; the updated comparison preserves upstream's six changed FA/EF objects explicitly and keeps the other 500 whole-file identical to that control. No animation source or canonical header workaround was needed.

External PR886's subsequent HBMBase metadata/source integration is likewise preserved on main `cc8923bd`. The repeated fresh 516-target default build, frozen verifier and complete animation/native/relocation audit all pass unchanged. Its new HBMBase object and HBMCommon header consumer remain explicitly attributed upstream.

## Traps

The native vtable's following alignment is not a tenth word of storage. Keep the object and split at 36 bytes and preserve the next unit's eight-byte start alignment.

The source compiler emits additional ordinary SDK helpers and a weak base vtable that this target does not retain. Check all final native function addresses, retained order, actual source-only ELF input and the full artifact rather than equating compiler-object offsets with linked offsets or adding artificial padding.

The precise version predicate and compiler-local string-table behavior are documented in [F-812-1](812-animation-interfaces.md). Neither observation justifies changed flags or overlooked code/data differences.
