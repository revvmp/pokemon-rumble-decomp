# F-795-3: Sequence sound source candidate

## Proven retained boundary

The SeqSound unit contains twenty-three retained functions from its constructor at 0x80280760 through its initializer ending at 0x80280F5C, totaling 1,864 instruction bytes. Its vtables occupy .data 0x803C6610–0x803C6660: SeqSound is 0x38 bytes and SeqLoadTask is 0x18 bytes. The .ctors entry at 0x80352A58 points to the initializer at 0x80280F50. The custom RuntimeTypeInfo is one four-byte base-type pointer at .sbss 0x804B9370; the split ends at 0x804B9374, leaving the following alignment gap outside this object. The canonical RuntimeTypeInfo declaration, native initializer store, and source-object extent establish four bytes rather than the original inferred eight-byte symbol extent.

The three small forwards at 0x80280C60, 0x80280C70, and 0x80280C80 are SetChannelPriority, SetReleasePriorityFix, and SetSeqUserprocCallback. Their native destinations and the canonical SeqPlayer signatures agree with the original reference and candidate methods. The two vtables and RTTI object receive their canonical class names. No outside source or split is taken.

## Split baseline

The fresh isolated build/795-seqsound-entry-baseline output includes the already reconstructed handle source and the original SeqSound fallback split. Configure, all_source, and the actual custom-directory DOL target exited 0; explicit frozen verification exited 0. This establishes the unit boundaries and metadata without selecting the unaccepted SeqSound source.

## Source fidelity limitation

The pinned original-library reference and canonical-only diagnostics are recorded in [F-795-1](795-seqsound-interfaces.md). The twenty-two other retained SeqSound functions reproduce their initial native instruction words and extents. Its natural constructor has the correct 0x9C extent but four store-order differences: native loading and prepared byte stores precede temporary-handle, manager, and start-offset stores, while the natural constructor schedules the prepared store later. Initial instruction comparisons do not replace paired-relocation, complete-object, or source-selected full-link acceptance.

The reference's explicitly fake pointer cast is excluded. Combined flag declarations, a named two-flag state object, ordinary constructor/body initializer variants, and compiler instruction scheduling do not resolve the difference in isolated diagnostics. Wii 1.0, 1.3, 1.5, 1.6, and 1.7 retain the same four-word constructor difference. Size optimization changes several otherwise exact functions and the constructor extent, so the established Wii/1.1 speed settings remain. These are bounded observations for this unit, not general compiler rules.

## Traps

Do not promote the initial twenty-two matching instruction streams into source credit while the constructor remains different. The entire SeqSound unit must retain its original fallback until complete object and real source-selected DOL acceptance succeed. The separate handle unit is independently accepted in [F-795-2](795-seqsound-handle.md).
