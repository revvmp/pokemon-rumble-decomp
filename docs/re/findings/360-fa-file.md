# F-360-1: First-owned native FA file unit

## Scope and ownership

The original #360 block begins with seven FAT functions and the separate four-byte PFFATFS_initializeFATFS unit at8020D920. The first retained function of pf_file.c is8020D930, the ninth original row. Pinned original FA order and45 corresponding retained native entries establish that the file ends80211FE0, supplying17 original #360 functions, all25 original #361 functions and the first three #362 entries. Those final public functions are cinsert, cdelete and flock. No preceding FAT split is taken merely through this file claim.

## Native diagnostic evidence

Ordinary C probes currently reproduce44 of45 retained file functions,17380 native bytes and351 independently resolved relocation sites in one object under unchanged Wii/1.0 SDK flags. These remain external diagnostic implementations, without source-selected acceptance or issue completion. The strict diagnostic rejects missing functions, differing instructions and unresolved relocation targets separately. GetSFD retains five differing instruction words; every other retained function matches.

The complete cursor read and write paths, file creation/open, private chain mutations, locking, open-state query, file finalization and all public wrappers match. The reproducible external commands are360-prepare-file-probe.py and360-audit-file-probe.py; the44-function snapshot is361-reference/file-candidate-44.c with its object, and the current audit writes361-reference/file-tfp2-candidate-proof.json. Independent helper review established that an iteration-local descriptor pointer preserves both the standalone IsOpened function and all inlined callers.

An external production cleanup preserves native definition order and removes unused functions from the other revision. It compiles against the canonical shared interfaces plus the proposed complete PF_INFO header, retaining the existing PFFILE_OpenEntry name and its unsigned mode contract. The pinned FA map calls that operation FsexecOpenFile; the name correspondence is explicit in the diagnostic rather than creating a parallel source alias. The production command360-prepare-file-production.py and strict file-production.o audit preserve all44 exact functions. Actual source selection, whole-unit allocated-section comparison and the full frozen build remain pending.

The primary related source is CC0 TFP2 b62dfe78759133a85bf5ce451bcf496b87439b0c, with file/header/license digests recorded in [F-361-1](361-fa-file.md). The CC0 Petari revision e0ab141914921726a595573dc48d3f13d0922efe also supplies the meaningful shared GetVolume helper used throughout file operations. Its inlining reproduces the native AdvanceToRead stream, whereas an equivalent directly initialized pointer gives different temporary-register allocation. This is an existing reusable reference interface, not a new one-use scheduling helper.

## Target differences

The native private sector routines take file/buffer/size/output, deriving their volume internally; the related VF revision instead accepts an additional volume argument. Each helper obtains its volume at the observed stage: read head after the zero-offset exit, read tail before its zero-size exit, and the composite read after the head operation. All four complete native read functions reproduce only with those actual interfaces and operations.

Read and write sector helpers preserve the error result when a partial transfer occurs, returning success only when the observed transfer count equals the intended count. The body-read comparison rounds its expected count down to a sector boundary. Removing these checks because the sector API usually completes a transfer changes both behavior and bytes.

MoveToEnd assigns the file size to the position and recalculates cursor fields. It does not perform the related SetPosition invalidation operation. Native cache-page allocation has four arguments, including an output hit flag. Public error guards also differ from the related source and are checked independently against each native operation.

The pinned reference declares PF_FILE.open_mode signed32, and native close uses a signed comparison. The bounded canonical correction and complete volume/entry controls are recorded in [F-363-3](363-fa-volume-interface.md). The PF_INFO probe uses32 bytes: file_size0, io_pointer4, empty_size8, allocated_sizeC, lock_mode10, lock_owner14, lock_count18 and lock_tcount1C, all directly populated by the exact native private info function. No canonical PF_INFO definition is published yet.

Native public fopen also compares its mode with signed cmpwi. Retaining the existing unsigned prototype produces one differing comparison; the pinned TFP2 prototype uses signed32. The proposed source/header correction changes no argument width or record storage and still requires current-consumer preservation before publication. Private filename length uses signed32 intermediates after the unsigned16 string API, reproducing the native signed division by13 and bounds comparisons in file creation.

The adjustment operation has a diagnostic private p_fadjust helper following the existing private read/write/append pattern: check write permission and locking, invoke FreeUnusedCluster, return an error or success. It naturally inlines to the observed native public adjustment body. This helper spelling and source organization remain a source hypothesis, not a recovered retained symbol, and require source-fidelity review before acceptance.

## Traps

- An existing library reference can use different private argument counts even when public signatures agree. Inspect actual argument registers before copying the call graph.
- Re-read instructions when pseudocode omits a live register assignment. The native append overflow path clamps count before continuing; the initial decompiler display omitted that move, and the strict byte audit rejected the unclamped source.
- Conditional pre-decrement of the descriptor reference count preserves the native loaded value through its zero test. Separate decrement and reload statements produce an extra load; the complete close path proves the original-looking expression without flags or volatile storage.
- Related VF write-engine bodies require four-argument cluster append, unlike the accepted FA three-argument interface. Reconstructing the actual FA allocation loop preserved the canonical contract and reproduced all three remaining write operations; do not alter a canonical callee contract to make another revision compile.
- Cursor cluster-end positioning assigns position and recalculates fields without SetPosition invalidation, like MoveToEnd. Its logarithm sum remains a32-bit value; narrowing that sum introduces an extra mask absent from the native stream.
- Mutation wrappers store an error separately after each cache flush. Combining those stores into one final error block changes their instruction streams. Insertion flushes FAT before data, whereas deletion flushes data before FAT; both orders are directly observed.
