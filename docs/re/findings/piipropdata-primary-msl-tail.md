# PiiPropData: original MSL boundaries resolve the last two functions

The original MSL vector initialization and pointer-copy boundaries jointly reproduce the last two PiiPropData functions. All 48 original function bodies, all 504 normalized function relocations, and all 26 original exception records and cleanup relocations are exact. The coordinator's normal source-selected build passes frozen full-DOL verification. This completes all 13,462 real CPU functions and source-links all 3,078,460 CPU code bytes.

## Primary source provenance

The reference is the unadapted September 2006 CodeWarrior distribution previously recorded in [the photo-saving findings](280-photo-saving.md#f-280-4-original-c-declarations-resolve-dialog-exception-ranges): [CW_GCN-3_0_Alpha5-2-060920.zip](https://archive.org/download/cw_consoles/Wii/CW_GCN-3_0_Alpha5-2-060920.zip). The newly recovered local archive is independently verified at 47,131,387 bytes with SHA-256 `6bac434a2ff28f75c44fdd9606b3d58c3ff302b081a8717e137417af1c98a515`. Its internal prefix is `CW_RVL/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include/`.

| Original header | SHA-256 |
| --- | --- |
| `vector` | `ed11b3e9b051ed59c78e57f853e985012814be2100f676ee6279aca5a508f3ee` |
| `algorithm` | `c3f40766e527dd5b68f66f280c94bae6aae6a647e1db624bef236115397af762` |
| `iterator` | `af238e8eae81f142045d9eddde1186e74a39876285cfa9694f4cb2a8287f2c08` |

The archive is retained privately at `tools/local/tail-20261104/vm_pii_primary/CW_GCN-3_0_Alpha5-2-060920.zip`. The vector reading copy is `vm_pii_primary/headers/4/vector`; algorithm and iterator are under `animcurve-pii-primary/reference/`. These original files are reading references, not wholesale replacements for shared reconstruction headers.

## Recovered call structure

The primary untransformed `__vec_constructor` copy constructor initializes its base with the source allocator, then calls the base range-initialization member with source `begin()`, `end()`, and a random-access iterator tag. It does not own the count calculation directly.

The actual count belongs to `__vec_deleter::init(ForwardIterator, ForwardIterator, forward_iterator_tag)`. That member converts `std::distance` to its unsigned `size_type`, tests for a positive count, then calls allocation and range construction. Its definition is outside the class and does not explicitly request inline. The existing reconstructed allocator, throw path, and exception hierarchy can remain unchanged.

The primary trivial range-construction body calls `std::copy`, then increments the vector size by an unsigned conversion of the original pointer difference. The const-pointer copy overload passes non-const pointer views to `__msl_copy<T>::copy`. Its POD specialization computes a separate unsigned `size_t` distance, calls `memmove`, and returns the destination plus that distance. This differs from the earlier source-local substitute, which cached a signed pointer difference before a direct `memmove` and reused it afterward.

Restoring both boundaries gives the compiler the original inline locals and pointer/count expression identities. It removes the need for the earlier caller-owned error-message output and allocation forwarding helper. The final source still needs the separately evidenced outer allocation-tag and short-vector growth controls; this result does not claim that every earlier D-0016 control has become unnecessary.

## Exact isolated evidence

The first exact packet is `tools/local/tail-20261104/game_PiiPropData/animcurve_pii_primary_copy_1_0_0/PiiPropData.cpp`, compiled with the private vector overlay `animcurve-pii-primary/animcurve_pii_primary_init_0_1_0/vector`. It uses the frozen `vmprop-targets/PiiPropData.o` and the configured compiler command. No shared build, shared header, expected hash, binary patch, or compiler-state modification is involved.

| Function | Original bytes | Raw SHA-256 | Function relocations |
| --- | ---: | --- | ---: |
| `PiiPropData::PiiPropData` | 1,048 | `a682167c8be54e2ac8e32faa90191e3f2e745bb7ead61cecdb3f43b38610d60f` | 53 |
| `PiiPropData::Reset` | 1,260 | `1bfc861988f84dd0239d43fee7939bf30d0a7d2d0a005ac112e2fee3a51ee665` | 58 |

All 48 original function byte strings agree, totaling 14,880 bytes. All 504 function relocation records agree after resolving compiler-generated literal names to their actual bytes and the two original shuffle labels to their `.sdata` locations and initializer words. In particular, original `lbl_803A91DC` contains the exact 55-byte exception descriptor emitted by the candidate. The two LevelMove shuffle states agree at `.sdata` offsets 4 and 8 with initial word `0xFFFFFFFC`; the second original symbol's extra four bytes are trailing alignment, not another shuffle value.

The original 256-byte constructor and 252-byte Reset exception records now agree completely, including the previously wrong receiver-register bytes at offsets `0x83` and `0xC7`. Every original cleanup relocation agrees, and all other original exception records remain exact. The candidate object has extra unselected template exception records, so the final source link must still verify which sections survive.

Receipts in the packet are `vm-full-audit.json` and `primary-raw-audit.json`. `animcurve_pii_primary_init.py` reproduces the member initialization experiments; `animcurve_pii_primary_copy.py` adds the original pointer-copy structure. `animcurve_pii_primary_audit.py` performs the independent raw-byte and normalized relocation comparison.

The final readable packet is `game_PiiPropData/jpeg_pii_exact_readable_1/`, using the `jpeg-pii-exact-readable-1/vector` overlay. It removes the obsolete address enum, caller-message throw helper, and allocation forwarding helper, and gives the temporary `construct_range` wrapper the primary `construct_at_end` name. An independent rerun of the raw/relocation audit and exception audit confirms the same 48 exact bodies, 504 exact normalized relocations, and 26 exact original exception records. The receipts are stored beside that final packet as well.

The coordinator then accepted the production source selection: configure and ninja both exited zero, and the complete DOL passed the frozen SHA comparison. No exact function regressed or disappeared. All 902 audited link aliases remain byte-identical. The actual link also validates the retained template sections, literal placement, and trailing shuffle alignment; no extra object exception records are mistaken for retained executable data.

The read-only compiler trace `game_PiiPropData/animcurve-pii-primary-exact.txt` confirms the changed scalar priorities. In the preceding near-match, the short-copy count was virtual 70 in `r21`, while the error receiver was virtual 64 in `r20`. In the first exact packet, the short-copy count is virtual 65 in `r20`, the error receiver is virtual 66 in `r21`, and the message remains in `r22`. The original receiver register is consequently also written into the exception cleanup record. The trace reads compiler state only; it does not change graph nodes or generated code.

## Why the earlier isolated controls were misleading

Applying only the original initialization member boundary produces 1,036 constructor bytes, twelve fewer than the target. Applying only the original pointer-copy boundary also produces 1,036 bytes, with a different byte string. Both isolated forms score about `97.18%`, worse than the prior `99.73%` register-only candidate. Applying both restores the required 1,048-byte instruction stream and reaches exact comparison. The corresponding Reset progression is likewise jointly resolved.

This is not evidence that an arbitrary extra inline level fixes every register swap. The primary headers establish the specific count owner, unsigned conversions, const-pointer copy forwarding, and nested helper calls. Those interacting boundaries restore the original arithmetic reuse and scalar creation order. Treating either isolated regression as proof against the primary source structure would have rejected the successful combination.

The preceding [coupled-output experiments](piipropdata-coupled-output-tail.md), [count-graph continuation](piipropdata-count-graph-tail.md), and [allocation-tag continuation](piipropdata-allocation-tag-tail.md) remain useful negative evidence. Their count identities were being tuned around a flattened library call structure. The recovered original structure supersedes those attempts for the final count/object pair.

## Traps

- **Restore interacting primary-source boundaries together.** Here the original vector initialization and optimized copy each regress alone but match together. Compare the complete evidenced call chain before rejecting a primary-source contract on one isolated score.
- **An instruction-identical hand expansion can still have the wrong inline identity.** The cached direct `memmove` substitute kept the native operation stream but placed count and error object in the wrong registers and unwind record bytes. The actual copy helper's unsigned count and pointer views resolve both without an exception-runtime workaround.
- **Exact original EH records do not prove all object sections are retained correctly.** The candidate contains extra unselected template records; full native link and frozen verification remain mandatory before unit acceptance.
