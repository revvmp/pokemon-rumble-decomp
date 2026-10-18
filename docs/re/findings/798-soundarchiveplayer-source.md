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

The split-only plain Ninja and explicit frozen verification pass after propagating the proven temporary-handle caller correction described below. Actual canonical-only NonMatching compilation and the fallback-selected full DOL also pass. The source is then selected as Matching: plain Ninja and explicit frozen verification pass, with all three retained object sections at 100%. The private interface context is not published and will be discarded after the owner prerequisites reach main. This establishes all thirty-four retained functions / 10,172 bytes in the approved interface context. Final publication excludes temporary interface copies and aliases, and final merge acceptance repeats on actual canonical main after the prerequisites.

## Traps

An imported callee name at the wrong native address is exposed only by paired relocation comparison. Initial masked instruction equality concealed SoundHandle's two attachment variants; use the source owner's proven correction and repeat the actual link, rather than treating the matching masked stream as acceptance.

The first split-only check failed at exactly one DOL byte, native 0x802824FB. Renaming SoundHandle attachment symbols changed ExternalSoundPlayer::ForEachSound's inline call from native C70 to CC0 in the existing Sound3DActor destructor. The root #799 owner confirmed that this temporary handle must call detail_AttachSoundAsTempHandle, accepted the exact one-identifier propagation with its canonical metadata, and required its own full consumer proof. The private context uses that approved correction; the repeated split-only build and frozen verification pass. It is an integration-name propagation issue, not a changed unit boundary.

## Source-selected acceptance

The actual project-compiled source object scores 100% in text, data and small-data constants. The two compiler-local constant aliases, @9641 and @9649, pair the source float and conversion double at their proven native addresses; these are compiler-local comparison aliases, not recovered original source variable names. They change no value or extent. The full linker preserves the four original intervening alignment bytes without adding source padding or byte arrays.

An independent actual-output audit checks all thirty-four compiled functions, final native ELF addresses/extents, and source linker-map contributions; all three vtables and both constant payloads have their exact native source-map entries. The generated Ninja link selects the source object and excludes its original fallback. Direct comparison of all 4,094,880 DOL bytes is exact and the frozen verifier passes. SHA-256 remains ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da; scope is the main executable only. Unretained methods and weak duplicates receive no additional credit.

## Canonical prerequisite integration checkpoint

After #837 and #851 merged, the public source stack was restacked onto
main c39785e81000d8b7ef5c85554564de8565e46e14 with the bounded #864 header
prerequisite. Compiling the actual production source with only this
checkout's canonical include tree reproduces all thirty-four native
functions / 10,172 bytes, all 315 independently resolved relocation targets,
eighty vtable bytes and twelve constant payload bytes. No pending-interface
or reference fallback include directory is used. This checkpoint confirms
that deduplicating the owner-approved #799 and #802 interfaces preserves
the source reconstruction. The final committed public stack, including the #864 header, was then
rebased onto current main and built from a clean build directory: `python
configure.py`, `.venv/Scripts/ninja` and frozen `tools/verify/verify.py`
all exit 0 with the unit source selected, and the report scores its text,
data and small-data constants at 100%.
