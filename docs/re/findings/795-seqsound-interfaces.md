# F-795-1: Sequence sound interfaces

## Scope and provenance

Issue #795 covers SeqSound at 0x80280760 through its initializer at 0x80280F50 and the retained SeqSoundHandle::DetachSound at 0x80280F60. This prerequisite supplies complete class interfaces without selecting source, changing splits, or claiming reconstructed code bytes.

The reference is zeldaret/ss commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, whose sequence sound files identify kiwi515/ogws as their original source. The pinned root license is CC0; the individual inspected files have no overriding license notice. Comments and reference-only scaffolding are omitted under this project's rules. Existing canonical BasicSound, SeqPlayer, FileStream, InstancePool, SDK, and utility interfaces are preserved. Main PR836 independently supplied SeqFileReader while this prerequisite was being prepared. Its canonical header is retained in full, with no competing SeqFile declaration; the consumer uses SeqFile::Header for the loaded file pointer.

## Native layout evidence

| Type or field | Native compiler result | Evidence |
| --- | --- | --- |
| BasicSound | 0x110 bytes | Existing canonical header and native base call |
| SeqPlayer | 0x164 bytes | Existing canonical header; embedded at SeqSound+0x110 |
| SeqSound | 0x4B4 bytes | Native constructor and methods address the following members |
| Temporary handle, manager | +0x274, +0x278 | Detach and priority-update native accesses |
| Sequence offset, start type, start offset | +0x27C, +0x280, +0x284 | Both Prepare overloads and load completion |
| Loading and prepared flags | +0x288, +0x289, volatile bool | Native byte operations and asynchronous flag accesses |
| File stream, stream buffer | +0x28C, +0x290 | Native stream preparation; 128 int elements, 0x200 bytes |
| SeqLoadTask | +0x490, size 0x24 | Native constructor, Execute, Cancel, and OnCancel |
| Task | 0x10 bytes | Vptr+0, list node+4, busy flag+0xC |
| TaskManager | 0x3C bytes | Three 12-byte lists, current task+0x24, flag+0x28, thread queues+0x2C/+0x34 |
| SoundInstanceManager<SeqSound> | 0x28 bytes | Priority list, pool, mutex; native priority-update instructions |
| SeqFileReader | 8 bytes | Header and data-block pointers; native reader operations |

The canonical headers compile with the established Wii/1.1 NW4R flags. All twelve retained TaskManager diagnostic consumer streams match their native instruction words and extents after relocation words are excluded. All three retained SeqFileReader consumer streams likewise match. These are interface diagnostics only; their source units remain owned by their corresponding issues. The target sequence-file reader accepts version 1.0, while the file format declaration retains the reference's 1.2 constant.

The SeqSound and handle diagnostic consumers reproduce 23 of the issue's 24 native instruction streams and extents (1,764 of 1,920 bytes). The remaining constructor has the correct 0x9C extent and member offsets but four different store-order words. This is not complete function matching, whole-object acceptance, relocation acceptance, or source selection. No unit is marked Matching by this header prerequisite.

## DVD cancellation declaration

SeqLoadTask::Execute at 0x80280DFC compares the read result against immediate -3. The pinned original SDK enum starts DVD_RESULT_COVER_CLOSED at -4 and then declares DVD_RESULT_CANCELED, proving -3; DVD_ECANCELED aliases that value. The existing canonical header incorrectly declared cancellation as -6 and omitted the original alias. This prerequisite corrects that declaration and adds the alias. No existing source references DVD_RESULT_CANCELED, as checked before the change. The actual source-object comparison and frozen DOL build check the existing consumers after this shared SDK correction.

## Reference adaptations

Task explicitly includes its existing NonCopyable base declaration. TaskManager uses the canonical SDK thread header. SeqFile uses the existing FOURCC and NW4R_VERSION macros, and consumers use the canonical Table::items spelling. FileStream::SEEK_ORIGIN_BEG is the existing name for origin zero. SoundInstanceManager spells the original iterator type explicitly instead of the reference's decltype convenience. The unretained debug-only GetSoundType helper is omitted because its enum is absent from the canonical debug interface; retained methods and all class data members remain declared. No debug SDK or unused compatibility interface is invented.

## Existing-source validation

Before editing shared headers, the isolated all_source build and actual custom-directory DOL build exited 0, and explicit frozen verification passed. After the header additions and DVD declaration correction, the same build again exited 0 and frozen verification passed. All 445 existing source objects have exactly the same SHA-256 digests before and after the change. Header consumer and native compiler layout diagnostics also exited 0. Final latest-main acceptance uses a fresh default-directory configure, plain Ninja, actual progress report, and explicit frozen verifier before merging.

## Canonical-only consumer check

An integration consumer exposed that the reference fallback include tree concealed an unavailable NW4RAssert.hpp include in SoundInstanceManager. The canonical header removes that include and the original release-disabled Assert/Check invocations, including the unused debug warning expression; all actual runtime statements remain. No substitute SDK header, debug interface, or no-op macro is introduced. Both diagnostic drivers now use only the repository include directory, with no reference or overlay include search path, and omit the same release-disabled diagnostic calls in their read-only source consumers. SeqSound/Handle still compile with 23 of 24 initial native streams exact, while TaskManager twelve and SeqFileReader three streams remain exact.

## Traps

The pinned SS and Xenoblade SeqSound constructors explicitly label a pointer-cast flag initializer as fake matching. The natural constructor has four store-order differences here. Importing that fake expression would satisfy bytes without meeting source fidelity; the source reconstruction must resolve the constructor independently or leave it unaccepted.

A missing SDK cancellation alias can conceal a wrong existing numeric declaration. The trigger here is the native -3 comparison versus canonical -6. Use the pinned original enum plus native instruction evidence, then verify all existing source outputs and the frozen executable; do not place a private literal or compatibility macro in reconstructed library source.

A diagnostic fallback reference include directory can conceal missing canonical dependencies even when existing-source Ninja succeeds. Remove all reference include search paths for the final consumer proof; future integration of a new template instantiation can expose headers unused by the existing build.
