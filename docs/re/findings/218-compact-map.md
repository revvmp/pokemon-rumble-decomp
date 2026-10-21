# F-218-3: compact map lookup and storage

The CWaza native constructor, destructor, count lookup/increment, and clear paths establish the compact map contract. Its twelve-byte storage has allocator/count, node allocator/root anchor, and comparator/begin state represented by three compressed pairs. The 2006 Metrowerks map/tree reading reference at Koo commit `11c112f7548b26ab2cecfcf52e201b8d3b76120f` documents these roles. The implementation is reconstructed independently against native producers, with unchanged Wii/1.1 game flags.

This prerequisite extends the current compact backend with standard key lookup, keyed default insertion, iterator equality, and swap. It preserves the current compact iterator dereference/increment implementation and the two current adapter classes introduced by #329. The generic tree, map interface, and operator[] path from #147/#148 remain unchanged. The keyed adapter forwards to its owning backend, consistent with the existing generic adapter API. No tree.h or vector declarations are changed.

All fourteen original CWaza functions / 4,128 instruction bytes retain their native extents and every direct branch target under this header. Its nine native exception records total 336 bytes and its complete vtable is 84 bytes. The source-selection and frozen-DOL gates remain separate requirements for the full #218 issue.

## Existing consumers

Current CLayout has 77 emitted functions and CMsgBalloon has 602. Every function extent and instruction stream remains unchanged. Each complete allocated section, including exception/data sections, and every symbolic relocation is identical between baseline and candidate. Anonymous compiler numbering is normalized by section, offset, and extent, including function-name string counters. Both current generic and compact map consumers are therefore controlled; the new iterator/adapter APIs are preserved.

Raw baseline/candidate objects, all-current-consumer-control.json, and all-section-current-consumer-control.json are preserved in `C:/MassStorage/GitHub/218-current-compact`. The independent CWaza direct-target audit is `C:/MassStorage/GitHub/218-control-audit.py`.

The complete prerequisite build passed configure (0), ninja (0), and the explicit frozen-manifest verifier (0), with a freshly generated main.dol. No source coverage was added by this header prerequisite.

## Traps

Adding a dispatch helper around operator[] preserved the large caller but changed an accepted out-of-line helper from eighty bytes to a four-byte forwarding branch. That proposal was rejected. The existing keyed tree API from #148 permits the compact addition without changing the generic call path. Preserve the complete caller graph and weak helper streams, not only the enclosing class function.

Old overlays can omit newly accepted iterator and adapter declarations. This proposal starts from current main and retains the #329 additions, rather than copying the earlier complete map header over them.
