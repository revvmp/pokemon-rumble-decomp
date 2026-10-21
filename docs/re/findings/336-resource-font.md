# F-336-3: Resource-font loading unit

The independently reconstructed CResFont::Load at `0x801D9A10` is now source-selected and exact: one function, 292 instruction bytes, seven independently resolved instruction targets, 88 exception bytes and one twelve-byte exception index. Configure, full Ninja, explicit all_source, map inspection and frozen-manifest verification exit zero. Eleven of the original #336 functions remain outside the accepted font units; this is partial issue work.

## Boundary and ownership correction

The original survey grouped `0x801D9A10` with the following fonts, while the initial #336 investigation conservatively treated it as preceding resource-font context. Further native evidence identifies the preceding function `0x801D99E0–0x801D9A08` as the CLayout static initializer: it zeros the global callback owner and registers its destructor. The preceding allocated data through `0x803C0FB0` contains CLayout's trailing default_delete and exception names, including ArcResourceLink at `0x803C0F30`; it is not a resource-font vtable. CRomFont's independent unit then starts at `0x801D9B40`.

The one strong resource-loader function lies between those units. Its independent CResFont.cpp source-file boundary is an inference supported by the initializer, trailing data cluster, separate next font unit, lack of emitted own data, and exact whole-source link. It owns only `0x801D9A10–0x801D9B34`, leaving the following twelve alignment bytes outside. No #335 code, split, class implementation or shared CLayout storage is taken.

Earlier CResFont weak definitions were retained from main.cpp context. Native RTTI at `0x804B7948` identifies CResFont using its name at `0x803B5F98` and base array at `0x803B5FA4`. The original vtable at `0x803B5F38` has the SDK ResFont inherited slot order except its derived destructor at `0x801313E0`. An initial scratch reading associated `0x801314C0` with that slot; direct vtable decoding rejects that association. Those earlier records remain outside this unit and are not renamed, split or selected here.

## Complete class contract

The constructor at `0x8012D910` calls the canonical SDK ResFont constructor, stores the derived vtable, and zeros the object/control owner pair at `0x1C/0x20`. Its 72-byte native body agrees with a complete 0x24-byte class consisting of the canonical 0x1C-byte ResFont base followed by shared_ptr<CFileData>. The 156-byte destructor at `0x801313E0` releases that owner and calls the SDK base destructor. LoadResourceFile's typed return and the loader's assignment establish the pointee type. The new header supplies declarations for the earlier constructor/destructor and preserves all inherited virtual methods in SDK order; it emits no new earlier implementation or data.

Load first removes the current SDK resource, loads the requested file with flags `0x4000` and the caller's background flag, and assigns the shared owner. It returns false for an empty owner or invalid file; otherwise it installs the file data through ResFont::SetResource and returns true. Load is a descriptive method name; the class spelling is recovered RTTI.

## Acceptance evidence

| Section | Original interval | Exact source-selected bytes |
| --- | --- | ---: |
| `extab` | `0x8003E964–0x8003E9BC` | 88 |
| `extabindex` | `0x80051990–0x8005199C` | 12 |
| `.text` | `0x801D9A10–0x801D9B34` | 292 |

There are no own initialized-data or small-data allocations. The source object's extra shared_ptr<CFileData> destructor and exception record are naturally discarded as duplicates of the canonical earlier instance at `0x800652D0`. Both retained cleanup pointers resolve to that original target. The map attributes the loader and all three retained section intervals to CResFont.o, and the actual link selects `build/wpse-v1/src/game/CResFont.o` without an original CResFont object.

The outside scripts `336-compile-resfont.py`, `336-audit-resfont.py`, and `336-resfont-linked-proof.py` preserve the compiler command and comparisons. The instruction audit resolves all seven targets before comparison, without masking instructions. The linked proof compares every allocated byte directly to immutable main.dol slices, queries Ninja's actual all_source inputs, and compares all 708 existing source objects against the pre-change baseline: 709 current objects including the new loader, zero differences in prior objects. `unitdiff.py game/CResFont.cpp` reports the sole owned function and all 292 instruction bytes at 100%. The larger object-level exception sizes include discarded duplicates and are not mistaken for retained differences.

The split was first verified with its original fallback object. After actual source selection, full map build, explicit all_source, and frozen-manifest verification pass with the old DOL deleted before building. No compiler flags, retention rules, byte substitutions, original hashes or frozen manifests change.

## Traps

- A class's earliest retained weak constructor, destructor or RTTI can belong to a consumer's object, while a later independent source file defines its strong method. Establish source-file ownership from file-boundary evidence rather than treating every earlier weak class instance as the first function of that source file.
- Decode the precise virtual slot before identifying a destructor. Nearby empty destructors can look identical while belonging to different classes.
