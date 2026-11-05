# U8 interpolation coalescing continuation

This private continuation follows [the U8 integer investigation](animcurve-u8-integer-tail.md). The selector, animation-curve and sort lanes owned separate experiments against the frozen `animcurve-original.o`. The animation-curve owner retains canonical source ownership; the coordinator owns complete-unit linkage and frozen executable acceptance.

## Starting graph

`selector_u8_type_half` has the native 4,832-byte extent, 1,208 instruction positions, exact floating-point instructions and exact branch words. Its 55 remaining native words are integer-register operands in four interpolation expansions, grouped as 21, 7, 20 and 7.

The fresh `selector_u8_type_half/trace.txt` identifies the first cycle. The end byte is virtual 145, the type is virtual 144, and the start byte is virtual 146. The start coalesces into the earlier interpolation return node, virtual 143. Descending coloring therefore gives end r3, type r8 and start/return r9; the target needs start/return r3, end r8 and type r9. The third expansion similarly merges start virtual 138 into return virtual 135, below type virtual 137, exchanging its required r10/r11 pair.

These are low-degree nodes. The issue is representative creation order after coalescing, not a spill threshold or a remaining arithmetic/scheduling difference. The [allocator rules](sprint-ra.md#interference-and-coalescing) explain why additional copies through inline `@` variables collapse into the oldest member.

## Third expansion completion

The animation lane first introduced `PrepareByteInputs`, which writes a loaded start through `u16&` and returns the loaded interpolation type as `u16`. Applied only to the first expansion, it fixes the type register and leaves the start/end pair exchanged.

The sort lane applied that same preparation to the third expansion. `sort_u8_thirdprep_u16_u16_0` reaches 99.85927%, with only 30 differing native words, grouped as 16, 7, 0 and 7. Using byte-width output storage regresses; using u16 or u32 output storage and a u16 or u32 returned type gives the same exact third expansion. Both caller declaration orders were equivalent in this bounded experiment.

The minimized `sort_u8_prepared_shared` packet reuses the existing helper, without a second definition. Its trace shows that the start still merges into the interpolation return, virtual 137, which now receives r10. The type survives as the later preparation return, virtual 129, and receives r11. Thus the successful control changes the type's surviving representative; it does not stop the start/return merge.

## Final first-expansion graph

The selector lane independently completes the two random-start expansions. The animation lane's first paired-input helper then leaves only ten words: end and type still exchange r8/r9. In `animcurve_u8_prepared_pair_0_0_1`, start/return virtual 146 already receives r3. The returned type is virtual 132 and receives r8, while an explicit byte reassignment leaves the end as later virtual 113 and gives it r9. The original caller's named `endValue` has degree zero, so permuting caller declarations cannot reorder these surviving inline temporaries.

The animation lane completes the pair by returning the end byte from the helper and writing the interpolation type through the byte output reference, followed by its same-type reassignment. In the exact `animcurve_u8_pair_return_end_0_0_1/trace.txt`, the first expansion is:

| Value | Surviving virtual register | Physical register | Edge count |
| --- | ---: | ---: | ---: |
| Start and interpolation result | 146 | r3 | 21 |
| End returned by preparation | 134 | r8 | 14 |
| Type retained by byte reassignment | 113 | r9 | 13 |

An independent comparison of the ordinary, uninstrumented candidate object finds all 4,832 unrelocated function bytes equal to the frozen target. Their SHA-256 is `df3c62172a086df6a53dd74a5ac8d5fad1f8accc667c46414a4b529f599fb33b`. The selector lane then removed the duplicated random interpolation body while preserving all bytes, and the animation owner prepared the complete unit.

The coordinator subsequently accepted the complete source replacement: the native link has zero differing executable bytes, and the ordinary configure/ninja pipeline with the unit selected as Matching passes the unchanged frozen DOL verification. U8 is therefore included in the accepted source-linked animation unit. The private graph traces above document the cause of the source controls; they are not substitutes for that separate complete-build acceptance.

## Rejected controls in this lane

- `sort_u8_return_width` widens the interpolation return while preserving explicit per-arm byte conversion. It leaves the input cycles unchanged and adds four masks after float conversions, increasing the function by 16 bytes.
- `sort_u8_return_identity` tries pointer round trips, typed local reassignment, and union OR/AND identities only on the returned start. Ordinary identities are neutral. Union identities retain native stores/operations, increasing the extent by 16 or 32 bytes.
- `sort_u8_start_objects` passes only the loaded start through byte/word objects, value/reference constructors and explicit copies. These are neutral or slightly worse. `sort_u8_caller_start` puts the loaded value in caller scalar, pointer or aggregate storage; scalar identities change colors but do not solve them, while aggregates are neutral or regress.
- `sort_u8_caller_return` adds actual caller return storage with scalar/pointer identity assignments. It does not change the surviving interpolation-return representative.
- `sort_u8_equal_end` returns the end on the equality path. Although behavior is equivalent there, it coalesces the return with the wrong live input and adds a move on the constant-start interpolation path. It is rejected.
- `sort_u8_paired_return` encodes the byte in a shifted u64 return and decodes it in the outer wrapper. It adds masks or wider arithmetic and does not preserve the native instruction sequence.
- `sort_u8_delayed_end` adds a separate end-return helper at four source positions, with plain return, reassignment or a repeated start output, and either direct-argument or separate-result calls. All 32 forms are neutral. Merely adding a helper does not guarantee a new surviving return identity.

## Traps

- A caller declaration can disappear while a later inline temporary preserves its value. Inspect the surviving graph object before permuting declarations; the first expansion's original `endValue` node is dead in the ten-word candidate.
- Coupled preparation can change which return node survives without changing any operation. Here it succeeds by moving the type below the interpolation return in coloring order, and finally by exchanging which value is returned versus written through the byte output.
- Capstone stops at unsupported paired-single instructions unless skip-data handling is enabled. The private audit explicitly enables it and checks all 1,208 positions; final acceptance also compares complete raw function bytes, so a prematurely truncated disassembly cannot report a false exact result.
