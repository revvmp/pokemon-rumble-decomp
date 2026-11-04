# CFileData final function and linking work

## Function match

On 2026-11-04, a fresh compilation reproduced the sprint baseline: 147 of 148 CFileData functions matched. `UpdateResourceFiles` differed only in twelve uses of the cache iterator register, r28 instead of r26. The previous honest attempts and allocator diagnosis are in [sprint-a02](sprint-a02.md#unsolved-after-the-fake-match-wave).

The retained D-0016 fake temporarily stores the worker pointer in the cache iterator's node pointer, casts it back to call `ProcessFinishedJobs`, and then passes that iterator by reference to an inline cache-purge overload. The purge overwrites the iterator before reading it as a cache node. The worker address is never dereferenced as a cache node.

Reusing the pointer first fixed the register allocation but moved five four-byte stack slots. Passing the iterator by reference through one inline level restored their original positions. By-value forwarding and two through four additional inline levels did not match. The original zero-argument purge remains for `ShutdownResourceFiles`.

`python tools/re/unitdiff.py game/CFileData.cpp` reports 148/148 functions and 63,272/63,272 function bytes at 100%. This is a function comparison, not a successful source link.

## Full build and remaining link work

An experimental `Matching` selection linked but failed the frozen SHA check. Exact verification reported the first differing DOL byte at `0xF032`. Linked symbol inspection found extensive emission-order differences, including the explicit map `erase` instantiation and its callees preceding the CFileData constructor. RTTI, vtable, and exception-table order also remain unresolved. The selection was not retained: CFileData remains `NonMatching`.

The main-checkout build with the retained function change and CLayout's separately recorded gain completed with ninja exit 0, followed by `verify.py` PASS against `config/targets/wpse-v1.json`. Original fallback selection remains in force for both units.

## Traps

Reusing a pointer through a local iterator can fix register allocation while changing stack-slot creation order. Inspect the entire function diff after a register improvement; the reference-forwarding inline level was necessary here. This is a targeted fake, not evidence that the original developers used these casts.

## Emission-order investigation

Scratch-only follow-up preserved all 148 function matches while improving the longest common subsequence of named function order from 110 to 125. No emission experiment below is retained in the source or shared headers yet.

Moving the forced map erase instantiation to the end, making the BThread destructor, Entry, and PushFinished inline, and repositioning the two worker helpers established the initial 116-order baseline. Replacing the forced erase instantiation with a local inline specialization preserved its code and put it in its caller's emission wave, but also requested allocator destruction too early.

Declaring CPath's copy constructor in a scratch header and defining it inline before OpenFileData moved its constructor and list-copy constructor into their required positions. The ordinary member initializer left the copy constructor at 99.43%; accessing the list through a character pointer advanced by sizeof(mDevice) reproduced all instructions. Local inline specializations of string-list pop_back/back and file-cache map subscript then placed their functions correctly. These are candidate fake matches only.

Adding an ordinary explicit vector assignment operator to a scratch vector header and locally specializing CNTHandleTable assignment placed that operator correctly but pulled several vector/tree helpers too early. The next task is to preserve delayed helper instantiation while keeping the early assignment operator. Snapshot source and comparisons are under ignored tools/local/session/310-ppu; mapspec1 and vecspec retain all 148 functions, with different ordering tradeoffs. The scratch header overlay includes CPath.h and vector and must accompany those candidates.

Native linker ordering was also investigated. The [NXP Power Architecture Build Tools Reference](https://www.nxp.com/docs/en/reference-manual/PA_Build_RM.pdf), pages 202?211, documents input file/section selection but no symbol-order selector within an input section. The Wii/1.1 command-line help and linker strings did not expose such a facility. No linker configuration was changed.
