# F-360-2: Native FA file information and mode interface

## Owned prerequisite

Issue360 owns the first retained pf_file.c function at8020D930 and its canonical pf_file.h. The complete file continues through80211FE0 and supplies neighboring361/362 functions. This bounded prerequisite changes no source selection, split, existing record storage or outside unit ownership.

## Native types

The private info operation at8020F960 writes all eight words of PF_INFO: file_size0, io_pointer4, empty_size8, allocated_sizeC, lock_mode10, lock_owner14, lock_count18 and lock_tcount1C. The resulting natural record is32 bytes. The native source probe reproduces all288 instruction bytes and resolved relocation targets with these fields. The canonical header retains every existing declaration, replacing only the opaque forward declaration with this complete record.

Public fopen at80210AC0 takes a signed32 mode. Its native comparison with2 is cmpwi. The unsigned prototype produces one differing cmplwi instruction; restoring signed32 reproduces all196 bytes and paired targets. CC0 TFP2 revision b62dfe78759133a85bf5ce451bcf496b87439b0c independently declares signed32 in include/private/vf/PrFILE2/fatfs/pf_file.h. Cached source/header digests are0ce79a3e5537b4732e0b9e7d831af2510b320f9b3fd6b13c530b4e15da5e68b3 andba14d549f93d0259f50c6f680e5067c097cf5dad45eba701230fe5e8db709f83. This changes no argument width or calling convention.

PFFILE_OpenEntry and its unsigned mode contract remain unchanged. The pinned FA symbol map names that operation FsexecOpenFile, but existing canonical consumers use OpenEntry; the source probe preserves the canonical spelling. FinalizeAllFiles is an additive declaration already approved by the same owner for the complete volume prerequisite; integration must retain only one identical declaration.

## Consumer preservation

The reproducible external command is python C:/MassStorage/GitHub/360-probe-file-header-consumers.py. It compiles current main pf_dir.c, the actual pending365 pf_fopen.c and368 pf_w_fopen.c twice with unchanged SDK Wii/1.0 flags and identical dependencies, changing only the file header. All three complete object files are byte-identical, including every allocated section, symbol table and relocation record. Persistent raw objects and proof are in361-reference/file-header-consumers/.

| Actual source | Original and proposed object SHA256 |
| --- | --- |
| pf_dir.c | 34c752f90fa4d6afb78ad794e96c207f73ad75a642d3aed2e2870cc1a4c5c9a8 |
| pf_fopen.c | 021a29adbd9c82e2de080fad850bca4ceccba7fe9b8a012324a141bc63738e76 |
| pf_w_fopen.c | 14f74dcb9cd97589108ab195c09c1c6f0f3b94403161ef187ed8a0611ff4ffeb |

The cleaned complete file probe preserves44 of45 exact native functions using these interfaces. The remaining GetSFD difference is unrelated to this header. Full current-existing-object preservation, committed-head default Ninja and explicit frozen verification remain required on the final prerequisite PR; these diagnostic checks alone claim no source progress or issue completion.

## Traps

- A related public mode prototype can use the correct width with the wrong signedness. Check the actual comparison instruction before inheriting unsigned types from forwarding wrappers.
- Preserve existing canonical names when an original map supplies another spelling; avoid parallel aliases that obscure the accepted consumer interface.
