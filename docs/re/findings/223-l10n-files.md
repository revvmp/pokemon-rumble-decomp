# Localized file loading (#223 and neighboring #224)

## F-223-1: original source identity and complete inventory

The original basename is `l10n_files.cpp`, independently encoded in the native RTTI names `@unnamed@l10n_files_cpp@::CL10NFileSet` and `@unnamed@l10n_files_cpp@::LessConstChar`. The mapped types are `std::map<const char *, CL10NFileSet, LessConstChar>` and `std::map<const char *, const char *, LessConstChar>`. The third owner is a `std::set<std::string>`. Native deleter strings occupy the data cluster beginning at `0x803B4638`; `main.cpp` starts its separate data at `0x803B4F80`.

The complete file starts at `0x801263F0` and its static initializer at `0x8012B550` ends at `0x8012B5E8`. Original #223 owns fourteen functions / 9,408 instruction bytes. Neighboring #224 owns the following forty functions / 11,320 instruction bytes. Together these are 54 functions / 20,728 instruction bytes. No individual subset is accepted as a complete original issue. The initial claim worktree is based on `2458a9fc5853bb7c6fb6058f8403e0e6db8f2137`.

The three shared owners occupy `0x804B90A0` through `0x804B90B8`, in the order interned paths, localized file sets, and selected archive paths. Their destructor-registration records start at `0x803E90D8`, `0x803E90E4`, and `0x803E90F0`. The final initializer registers the same owners in that order. Both map allocations are sixteen bytes; the intern set allocation is twelve bytes.

## F-223-2: native contracts

`InternPath` is a descriptive name for the first function: insert a narrow string into the persistent set, then return the stored string's `c_str()`. Keys retained by both maps therefore remain valid until the caches reset. `LessConstChar` compares string contents using `strcmp`; it does not order pointer addresses.

`CL10NFileSet` is the original anonymous-namespace type. Its twenty bytes comprise an unsigned count at zero and four path pointers at four. Default construction initializes the count and first null terminator. Its add operation accepts a file-list owner by value and a file owner by const reference; it rejects an empty or invalid file, interns the file's own path, appends the path and null terminator, and retains the file in the list. These types and parameter forms follow native accesses and reference-count changes.

`LoadedFileData` is an original global class name. Its allocation is twelve bytes, and its cleanup is the three-layer `std::list<shared_ptr<CFileData>>` cleanup. An empty public derivation from that list explains both layout and the distinct original default-deleter type. The canonical `FileList` alias and existing return interfaces are retained: local ownership can construct a `shared_ptr<FileList>` directly from `new LoadedFileData`.

`CPath` is an external dependency, not an owned declaration. Its constructor at `0x801AE670` proves a device number at zero, default unspecified value 540, followed by `std::list<std::string>` at four; the complete extent is sixteen bytes. This agrees with the earlier [CPath prefix evidence](313-cnt-file-stream.md#separate-cpath-prefix-investigation). The constructor at `0x801AE690` copies a supplied component list and device value. `0x801AEB60` builds the printable path; `0x801AF240` parses components. A temporary scratch declaration documents those interfaces without publishing a competing canonical header or accepting any external producer.

The path splitter at `0x80126810` copies the parsed components, moves preceding components into a second list until a component longer than four bytes ends in `.arc`, returns the interned remaining archive path, and writes the joined preceding components as the entry path. Joining an empty list returns an empty string; otherwise it prints a device-zero path without a device prefix and removes the first separator with a substring operation.

The list loader at `0x80127CF0` first consults the file-set cache. A cache hit passes its stored paths and requested flags to the multiple-file loader. A miss tries locale, region and common paths, retaining successful files; the common path uses the resource loader when the resource flag is set or an earlier path succeeded. It caches the resulting terminated path set and exposes the common `game2d.arc` set through the existing shared-layout-archives global. The scalar loader at `0x80128920` first consults the selected-path cache, otherwise resolves archive paths or tries the same locale/region/common sequence, caching its selected path.

## F-223-3: bounded outside reconstruction controls

No production source or shared header is changed at this checkpoint. The complete algorithm draft is `C:/MassStorage/GitHub/223-reference/l10n_files.cpp`, with ordinary classes and member functions and the existing Wii/1.1 game flags. Compiler entry point: `C:/MassStorage/GitHub/223-compile.py`. `C:/MassStorage/GitHub/223-whole-inventory.py` checks every original function against the compiled draft; its output explicitly reports masked candidates rather than strict matches.

The first complete draft produced relocation-masked candidates for 44 of 54 original functions. Ambiguous identical weak destructors remain candidates until their actual RTTI and relocation graphs are paired. Seven explicitly identified streams reproduce every non-relocation word: joining paths (156 bytes), three list-string destructor layers (204/208/212), cache initialization (2,512), cache reset (192), and the multiple-file forwarding thunk (4). The static initializer (152), string-size accessor (32), file-validity predicate (40), and several shared-pointer and tree helpers also have candidates. These results do not establish code, data, EH, placement or source-link acceptance.

The compatible outside #218/#219 tree/map proposal supplies distinct generic const iterators. Giving the set immutable iterators adds its missing pair conversion, but does not by itself reproduce the native interning context. The tree stores its comparator but reconstructs `Compare()` during searches. Using the stored comparator directly throughout insertion preserves state and naturally produces the native 408-byte interning extent; 39 unmasked words still differ in registers and stack offsets. An earlier iterator cache was removed. Delegating to `insert_one`, naming the result pair, spelling dereference explicitly, and obtaining the shared owner's raw pointer do not solve this context. Passing the string by value leaves the interning gap and substantially regresses its callers, so the const-reference signature is retained.

The cache initializer exposes an independent custom-comparator contract. The outside map proposal initially compiles the POD-mapped custom-comparator archive map as twelve bytes, whereas its native allocation is sixteen. Keeping `map_member_compare` for custom comparators, while limiting the stateless POD `map_value_compare` specialization to `std::less<Key>`, restores both native allocation and begin-pointer offset. The full 2,512-byte initializer then has no non-relocation differences. This also preserves comparator state as required by its interface. The #218 owner was informed; no canonical map change is made here and full consumer controls remain mandatory before any publication.

The canonical string currently lacks substring construction, `substr`, string-subrange `replace`/`assign`, and `out_of_range`. Bounded scratch additions establish the correct algorithm but remain unresolved in code generation: the natural subrange-replace form emits 472 bytes against the native 476. Direct duplicate size queries emitted 516; combining buffer/length reads removed that redundant work. Directly spelling a substring constructor in the path join incorrectly inlines it and grows that function from its exact 156 bytes to 612, so that source form was rejected. These experiments do not solve or modify the separate short-string-address-retention problem.

`C:/MassStorage/GitHub/223-strict-code.py` additionally checks instructions without masking opcode or register bits, resolves branch/SDA/data relocations to native targets, and recursively compares initialized RTTI/vtable graphs. The restored const-reference draft passes 22 of its 28 explicitly paired functions and 28 initialized data objects. These include all four actual deleter vtables and their get/dispose/deleting-destructor targets, both map destroy functions, and the initializer. Six selected functions remain unresolved. This is bounded function/data evidence, not whole-unit EH, placement or source-only acceptance. Wii/1.1 and Wii/1.3 controls produce identical bytes and attributes for all eight allocated sections; their differing object metadata provides no compiler-version lead.

## F-223-4: original split control

The complete original unit is now explicitly split, without registering a reconstructed source object. Its ranges are:

| Section | Start | End |
| --- | --- | --- |
| extab | 8002C2F0 | 8002CBC8 |
| extabindex | 8004C920 | 8004CB18 |
| .text | 801263F0 | 8012B5E8 |
| .ctors | 803529EC | 803529F0 |
| .data | 803B45B8 | 803B4F80 |
| .bss | 803E90D8 | 803E90FC |
| .sdata | 804B7840 | 804B7880 |
| .sbss | 804B90A0 | 804B90B8 |

The exception index has 42 records / 504 bytes, referring to 2,264 extab bytes. The ctor entry points to the native initializer. The three twelve-byte registration records consume 36 BSS bytes; correcting the inferred first symbol extent from forty to thirty-six leaves the following four padding bytes with the next automatic object. No source array supplies that padding. `python configure.py`, the actual incremental Ninja build after deleting only `main.dol`, and explicit frozen-manifest verification all exit zero. `main.dol` equals the frozen original. This proves the split only: the unit remains original-object fallback and neither original issue is complete.

## Traps

- **A file-loading survey label hides an original basename.** The anonymous class spelling in the deleter RTTI identifies `l10n_files.cpp`; using a guessed basename changes every local class name even when ordinary code matches.
- **POD mapped values do not make a custom comparator stateless.** Selecting an adapter that recreates `Compare()` loses the stored comparator and changes this target's map extent. The two native map allocations and complete initializer provide a bounded control, not permission to alter unrelated map backends.
- **Small size gaps are not strict matches.** The first interning draft can have the exact 408-byte extent while retaining 39 register/stack differences. The whole-inventory diagnostic masks relocation words and permits ambiguous candidates; it must never be reported as native acceptance.
