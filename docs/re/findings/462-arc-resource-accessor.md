# F-462-5: ResourceAccessor and MultiArcResourceAccessor

Target: immutable `wpse-v1` `orig/wpse-v1/main.dol`. The user approved covering the shared ArcResourceAccessor files across #461 and #462. The reconstructed units begin in #461 and provide the first ten functions of #462; the earlier animation unit in #461 remains outside this work.

## Units

| Unit | Section | Exact range | Contents |
| --- | --- | --- | --- |
| `nw4r/lyt/lyt_resourceAccessor.cpp` | `.text` | `0x802B0680–0x802B06D8` | destructor, constructor, default `GetFont` |
| | `.data` | `0x803C7250–0x803C7264` | ResourceAccessor vtable, 20 bytes |
| `nw4r/lyt/lyt_arcResourceAccessor.cpp` | `.text` | `0x802B06E0–0x802B0C30` | two archive helpers, FontRefLink constructor and Set, ArcResourceLink Set, MultiArcResourceAccessor constructor/destructor/Attach/GetResource/RegistFont/UnregistFont/GetFont |
| | `.data` | `0x803C7268–0x803C727C` | MultiArcResourceAccessor vtable, 20 bytes |
| | `.sdata` | `0x804B8A20–0x804B8A27` | `"."` and `".."` path strings, the latter at `0x804B8A24` |

The two units contain 15 native functions and 1,388 code bytes: three functions/88 bytes in ResourceAccessor and twelve functions/1,300 bytes in ArcResourceAccessor. Padding between the vtables and after the last path string belongs to the following object. No original bytes or hashes are changed.

## Source and compiler evidence

The CC0 zeldaret/ss sources `src/nw4r/lyt/lyt_resourceAccessor.cpp`, `lyt_arcResourceAccessor.cpp`, and their public headers at commit `57a709b2128eb163a18e2969d561c9abb7d1fcee` provide the interfaces and bodies. Only functions present in this target unit are retained. The single-archive ArcResourceAccessor methods in that reference are absent from this target unit and are not imported. The target additionally emits `MultiArcResourceAccessor::UnregistFont`, an eight-byte tail call to `LinkListImpl::Erase` on the font list; it follows the reference's equivalent ArcResourceAccessor method.

Wii/1.1 with the established `cflags_nw4r` reproduces every non-relocation instruction word of all fifteen functions at their target addresses. The comparison masks only actual source-object relocation words, preserving function-relative branch offsets. The source's virtual-table relocation slots agree with the native tables: destructor at `0x08`, `GetResource` at `0x0C` (zero for the abstract base), and `GetFont` at `0x10`. Source objects also contain unreferenced generated weak destructors; those are discarded by the linker and receive no new reconstruction credit.

Both units are `Matching`. After the split-only configure/Ninja/explicit frozen verification each passed with exit zero, objdiff confirmed 100% in every owned section and all fifteen native functions. The source-selected configure/Ninja/explicit verifier also each exited zero; the actual `build/462-arc-split/wpse-v1/main.dol` directly equals the immutable reference. Ninja's real `main.elf` inputs select the two `src/nw4r/lyt/` objects, not their `obj/` fallback counterparts. The unrelated HBM source inputs are retained.

## Class layouts

| Type | Size | Members |
| --- | --- | --- |
| `ResourceAccessor` | `0x04` | vtable pointer |
| `FontRefLink` | `0x8C` | list node `0x00`, 128-byte font name `0x08`, font pointer `0x88` |
| `ArcResourceLink` | `0xA4` | list node `0x00`, 28-byte ARCHandle `0x08`, 128-byte resource root `0x24` |
| `MultiArcResourceAccessor` | `0x1C` | ResourceAccessor base, archive list `0x04`, font list `0x10` |

FontRefLink's constructor clears both node pointers and its font pointer. Set copies the font name and stores the font at `0x88`. ArcResourceLink::Set passes `this + 0x08` to ARCInitHandle, copies at most 127 root-name bytes to `this + 0x24`, and terminates at `0xA3`. MultiArcResourceAccessor's constructor establishes archive and font sentinels at `0x08` and `0x14`; resource lookup walks the archive list and font lookup compares registered names. Names and layouts are high confidence from the reference and matching target accesses.

## Traps

- **Recursive helper inlining changes both routines.** Removing the reference's explicit no-inline attribute lets `-O4,p -ipa file` expand FindNameResource from 204 to 460 bytes and GetResourceSub from 320 to 704 bytes. Preserve it with the canonical `NO_INLINE` macro; that restores both exact instruction streams without changing compiler flags.
- **A repeated source basename can repeat anonymous helper names.** The HBM and NW4R copies both mangle the global anonymous helpers with `lyt_arcResourceAccessor_cpp`. Mark the reconstructed anonymous symbols local in `symbols.txt`, preserving the unrelated HBM unit and resolving each source object's calls locally.
