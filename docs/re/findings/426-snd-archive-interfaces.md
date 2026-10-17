# F-426-3: Archive interfaces for retained NAND stream methods

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. This prerequisite supplies the complete canonical `NandSoundArchive` declaration needed by issue #426. The identical `SoundArchive`, `SoundArchiveFileReader`, and archive file structures arrived on main in PR #786 while this prerequisite was pending; the rebase preserves those canonical headers without duplicate additions. It changes no source selection, splits, symbol configuration, or credited code bytes.

## Native interface evidence

Compiling the pinned original `SoundArchive.cpp` and `SoundArchiveFile.cpp` against these declarations with Wii/1.1 reproduces five and 23 uniquely identified native instruction streams respectively, excluding relocation words. These diagnostic consumers remain outside the repository and claim no reconstruction progress. Examples include `SoundArchive` construction at `0x80283E10`, `IsAvailable` at `0x80283E80`, `Setup` at `0x80283EA0`, and `detail_OpenFileStream` at `0x80283FD0`. Reader construction at `0x80284200`, initialization at `0x80284230`, and string-chunk initialization at `0x802842E0`, together with the information readers and reference traversal routines, establish the field accesses and file structure layouts. A later source issue must separately prove complete objects, relocations, splits, source selection, and final DOL identity.

The ten eight-byte weak methods at `0x8027E210` through `0x8027E2A0` belong to `ut::NandFileStream`: native vtable `0x803C6988` binds each method to its corresponding virtual slot, and `GetRuntimeTypeInfo` uses the type information object at `0x804B93D0`. The canonical NAND stream header already defines these original inline methods. A faithful compilation of the pinned `NandSoundArchive.cpp` naturally emits all ten in the native order and with exact extents and instruction bytes. No forcing helpers, copied machine instructions, or synthetic assembly are needed.

The pinned reference links `MmlSeqTrack`, `MmlSeqTrackAllocator`, `NandSoundArchive`, `PlayerHeap`, and `RemoteSpeaker` in that order. The target has the same surrounding MML, allocator, heap, and speaker functions. The retained class identity and all ten method bodies are proven; `snd_NandSoundArchive.cpp` as the target's original host translation unit remains a hypothesis because its strong methods are unretained. That limitation must remain explicit in the source acceptance. Unretained archive methods receive no credited bytes.

## Provenance and adaptations

Declarations follow CC0 `zeldaret/ss` commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`. The root license and individual files were inspected and contain no overriding file-level notice. Includes use the existing canonical NW4R, MSL, and RVL_SDK headers. Existing `detail::PanMode` and `detail::PanCurve`, `u8`, `FOURCC`, `NW4R_VERSION`, and a constant expression for header alignment replace equivalent reference vocabulary. Reference comments and analysis annotations are excluded from source. C++ null pointers use `nullptr`.

The reference gives the byte at offset six in both 3D parameter structures an offset-only placeholder. It is named `spatialParameter` here: its position, width, and copying through `ReadSound3DParam` are proven, while its original name and precise meaning remain unknown. This descriptive name claims only membership in the spatial parameter block. The diagnostic consumer uses the same spelling on both sides of the copy and retains its native match.

The diagnostic NAND consumer omits the two unretained nested stream constructor definitions whose SDK constructor interface differs from the canonical target revision. Their declarations remain original; no invented SDK overload is added. Full source acceptance must demonstrate that these constructors and all other unretained methods are discarded and that the ten retained methods come from the newly selected source object.

## Validation

At main baseline `2b629bb7d8c094c370c848feaccc873a2d81ab70`, `python configure.py --no-progress --build-dir build/426-archive-header-baseline` and `.venv/Scripts/ninja all_source` exited zero. After adding the headers, `ninja all_source` exited zero and all 411 existing source-object SHA-256 values remained identical. The actual custom-directory DOL target built successfully, and explicit frozen verification of that artifact exited zero. Both diagnostic consumers compile against the actual canonical headers with the same five plus 23 unique native matches. The pull request records the final default-directory full gate after rebasing onto latest main.

## Traps

An existing matching `ut_NandFileStream.cpp` can emit the same weak methods while the linker continues selecting earlier original fallback definitions. Native byte identity alone does not demonstrate source coverage. The source change must remove the ten methods from their fallback split and inspect the actual selected object inputs and final native symbol extents.
