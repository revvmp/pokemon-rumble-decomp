# F-229-1: main startup shared contracts

This prerequisite supports the original #229 main.cpp ownership functions and the neighboring first-owner #225 main.cpp source. It does not complete either issue or add an archive-font source split.

## Complete CArchiveFontWithBuf

Native RTTI names CArchiveFontWithBuf. The constructor at 801CA130 has 72 bytes, the destructor at 801CA180 has 184, and Load at 801CA240 has 516. The complete class derives from the canonical 32-byte nw4r::ut::ArchiveFont and contains shared_ptr<u8> mBuffer at 32, totaling 40 bytes. Its complete ordinary outside producer reproduces every non-relocation instruction word of all three functions and all 348 exception bytes (8, 92 and 248), including their cleanup targets.

Load obtains a temporary CFileData through LoadResourceFile, checks its validity, obtains the required ArchiveFont buffer size, allocates from MEM2 with alignment 32, and stores that allocation with GameMemory::Free. Native buffer control vtable 803A42AC is the already named shared_ptr_deleter<Uc,PFPv_v>; destructor cleanup calls the already established shared_ptr<u8> destructor at 800657E0. This distinguishes the buffer member from the temporary CFileData owner. The ordinary destructor calls Destroy when mBuffer.get() is non-null. This completes the previously bounded type evidence in F-328-2 without importing an outside implementation.

The actual main font loader independently has 1,028 exact instruction bytes and 444 exact exception bytes against this complete type, the canonical CResFont and PairFont types, and the current FontMap. The unrelated FontMap ownership cleanup still differs and remains outside this prerequisite.

## SetMsgRes parameter lifetime

The native message initializer at 8012BA70 creates a CMsgRes shared owner, copies that owner into a distinct function argument, calls SetMsgRes and destroys both argument and local owner. The same argument-copy operation occurs in the main Run path at 8012CA40. SetMsgRes therefore receives shared_ptr<CMsgRes> by value. The earlier const-reference declaration missed this caller-owned argument lifetime while still reproducing the callee instructions.

Using the corrected declaration, the ordinary actual initializer reproduces all 380 native instruction bytes and 144 exception bytes. With the const-reference declaration it has 312 bytes, omitting the copied argument and its cleanup. The initializer uses the existing REGION_EUR enum and SC_LANG_ENGLISH SDK constant and preserves the original date, time and resource tables.

The actual Message.cpp callee retains all 164 native instruction bytes with the by-value definition. Comparing its complete object against the previous source object preserves every allocated-section byte, section size and symbolic relocation, after normalizing only the deliberate SetMsgRes signature rename and anonymous-symbol identities by section, offset and extent. No other current reconstructed source calls SetMsgRes. The outside native producer additionally emits the CMsgRes deleter lookup, disposal and shared-pointer destructor naturally; no explicit instantiation or retainer is introduced.

The only symbol change renames the existing 801EF970 function. It does not alter that outside-owned symbol's extent, type, split or configuration.

## Validation

Outside reproducible controls are retained at C:/MassStorage/GitHub/229-reference. main-fonts.cpp contains both real main startup helpers. archive-font.cpp contains the complete ordinary header producer. archive-audit-code.py and archive-audit-eh.py record the three native archive-font functions and exception records. audit-code.py and audit-eh.py retain the main helper controls and remaining negative FontMap comparisons. audit-message-contract.py verifies the unchanged complete Message.cpp object. No outside producer body is published by this prerequisite.

On current main 3901366fbabe3939ba9e74f8339f6091066f7836, python configure.py, a fresh-DOL ninja build and frozen-manifest verify each exited 0. The resulting DOL retains SHA-256 ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da. Official landing repeats these gates on the exact latest-main commit. No source unit is newly marked Matching, and this document makes no original issue completion claim.

## Traps

An existing callee can match with either a by-value or reference shared-pointer declaration while its callers expose the missing argument lifetime. Check both the complete callee and real callers; matching a callee alone does not establish the public parameter contract.

A buffer ownership member and a temporary loaded-file ownership member have the same eight-byte storage but different element types and deleters. The native allocated buffer control, destructor cleanup and complete producer distinguish them. Do not infer the element type from identical shared-pointer destructor bytes alone.
