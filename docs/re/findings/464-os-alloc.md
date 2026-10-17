# F-464-2: OSAlloc unit and OSAlarm source link

Target `wpse-v1`, `main.dol`. Tools: decomp-toolkit 1.8.4, objdiff-cli 3.8.2, CodeWarrior Wii/1.0 with the F-0007 SDK flags at default alignment.

## OSAlloc boundaries

`RVL_SDK/os/OSAlloc.c` covers `.text` `0x802B3810`-`0x802B3A38`, `.sdata` `0x804B8A58`-`0x804B8A60` and `.sbss` `0x804B9490`-`0x804B9498`. Only `DLInsert`, `OSAllocFromHeap` and `OSFreeToHeap` survive linking. `OSArena.c` begins at `0x802B3A40`, so the whole unit lies inside #464. `__OSCurrHeap` is the only `.sdata` object and `HeapArray` the only kept `.sbss` object; `DLAddFront` and `DLExtract` are inlined.

The source follows Petari's `OSAlloc.c` unchanged in behavior. All three functions and both data sections compare at 100%, and the full DOL verifies with the unit linked.

## OSAlarm promotion

The three external names that kept `OSAlarm.c` `NonMatching` in F-464-1 now exist in `symbols.txt`: `__OSTimeToSystemTime`, `__DVDTestAlarm` and `__div2i`. With the unit marked `Matching` the link succeeds and the full DOL verifies.

## Traps

- **A split from original bytes can lose its function alignment.** With `OSAlarm.c` linked from source, the following `OSAlloc.c` split, still built from the original bytes, started at `0x802B3808` instead of `0x802B3810`. That shifted every later function, and the DOL check failed at file offset `0x263AC8`. Adding `align:16` to the OSAlloc `.text` split line fixed it before the source was linked.
