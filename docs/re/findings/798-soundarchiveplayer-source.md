# F-798-2: SoundArchivePlayer source unit

## Native scope and boundaries

The complete retained unit contains all thirty-four #798 functions and 10,172 instruction bytes. Text starts at 0x80285400 and ends at the final destructor adjustment thunk's end, 0x80287C68. The next SoundHandle function starts at 0x80287C70 and remains outside this split. Three vtables occupy 0x803C6738–0x803C6788. The following NintendoWare banner belongs to the following source context and is excluded. The retained constants are float 127.0f at 0x804BAFD8 and signed-integer conversion double 0x4330000080000000 at 0x804BAFE0; four intervening alignment bytes are preserved, so the constant extent ends at 0x804BAFE8. No BSS or constructor-table entry is owned.

All eight newly named text functions follow the canonical constructor, callback destructors and original LinkList specialization signatures. Callback vtables pair the two destructor addresses with their respective NoteOn and GetWaveSoundData methods, resolving the original survey placeholders. The three vtables retain canonical class names.

## Source fidelity and diagnostic evidence

The source follows the pinned CC0 original-library reference in [F-798-1](798-soundarchiveplayer-interfaces.md), preserving normal C++ class methods, templates, virtual order and member initialization. No compiler flag changes, assembly, raw instructions or forcing constructs are used. Unretained LoadGroup definitions are omitted because they require an unrelated unproved archive-loader implementation; all retained methods and natural inline helpers remain. Native release-disabled assertions, checks, warnings and panic calls are omitted, preserving diagnostic-only control flow. Canonical byte and table-item spelling replaces reference-only type aliases without changing their types.

A strict independent diagnostic resolves 315 relocations using native source-owner symbols, actual SDA bases and original target values. All thirty-four functions are byte-exact at their original addresses; all eighty vtable bytes and twelve constant payload bytes are exact. This diagnostic placement does not establish actual source-selected whole-link acceptance.

Pending canonical names are reused from their owners: #799 corrects the previously imported detail_AttachSound name to 0x80287CC0 and names the temporary-handle variant at 0x80287C70. #795 owns the three SeqSound forwarding methods. #426 owns the MmlSeqTrackAllocator vtable. No outside source, split, symbol extent or type is taken.

## Pipeline stages

The pre-split default context containing the approved canonical pending interfaces passed a clean configure --map, plain Ninja and explicit frozen verification. It uses project headers only. The original source baseline has 498 objects; six raw object hashes changed after intervening main header changes and completion of SoundArchivePlayer. All six retain 100% text comparisons, and the complete actual source-linked DOL is byte-identical. Raw object identity is not claimed for this larger header context.

Split-only verification, NonMatching object comparison and actual source-selected acceptance are the next ordered gates. The private interface context is not published and will be discarded after the owner prerequisites reach main. No code credit is claimed at this stage.

## Traps

An imported callee name at the wrong native address is exposed only by paired relocation comparison. Initial masked instruction equality concealed SoundHandle's two attachment variants; use the source owner's proven correction and repeat the actual link, rather than treating the matching masked stream as acceptance.
