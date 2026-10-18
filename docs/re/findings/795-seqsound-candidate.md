# F-795-3: Sequence sound source candidate

## Proven retained boundary

The SeqSound unit contains twenty-three retained functions from its constructor at 0x80280760 through its initializer ending at 0x80280F5C, totaling 1,864 instruction bytes. Its vtables occupy .data 0x803C6610–0x803C6660: SeqSound is 0x38 bytes and SeqLoadTask is 0x18 bytes. The .ctors entry at 0x80352A58 points to the initializer at 0x80280F50. The custom RuntimeTypeInfo is one four-byte base-type pointer at .sbss 0x804B9370; the split ends at 0x804B9374, leaving the following alignment gap outside this object. The canonical RuntimeTypeInfo declaration, native initializer store, and source-object extent establish four bytes rather than the original inferred eight-byte symbol extent.

The three small forwards at 0x80280C60, 0x80280C70, and 0x80280C80 are SetChannelPriority, SetReleasePriorityFix, and SetSeqUserprocCallback. Their native destinations and the canonical SeqPlayer signatures agree with the original reference and candidate methods. The two vtables and RTTI object receive their canonical class names. No outside source or split is taken.

## Split baseline

The fresh isolated build/795-seqsound-entry-baseline output includes the already reconstructed handle source and the original SeqSound fallback split. Configure, all_source, and the actual custom-directory DOL target exited 0; explicit frozen verification exited 0. This establishes the unit boundaries and metadata without selecting the unaccepted SeqSound source.

## Source fidelity limitation

The pinned original-library reference and canonical-only diagnostics are recorded in [F-795-1](795-seqsound-interfaces.md). The twenty-two other retained SeqSound functions reproduce their initial native instruction words and extents. Its natural constructor has the correct 0x9C extent but four store-order differences: native loading and prepared byte stores precede temporary-handle, manager, and start-offset stores, while the natural constructor schedules the prepared store later. Initial instruction comparisons do not replace paired-relocation, complete-object, or source-selected full-link acceptance.

The reference's explicitly fake pointer cast is excluded. Combined flag declarations, a named two-flag state object, ordinary constructor/body initializer variants, and compiler instruction scheduling do not resolve the difference in isolated diagnostics. Wii 1.0, 1.3, 1.5, 1.6, and 1.7 retain the same four-word constructor difference. Size optimization changes several otherwise exact functions and the constructor extent, so the established Wii/1.1 speed settings remain. Wii 1.0a, 1.0RC1, and 0x4201_127 also preserve all twenty-three other streams and the same four constructor differences. Deferred auto inlining leaves the same four differences; disabling file IPA changes two otherwise exact streams and shortens the constructor to 0x7C, while disabling scheduling leaves only twelve of twenty-four candidate streams exact. None of those diagnostic options is applied to project configuration. These are bounded observations for this unit, not general compiler rules.

## Canonical pipeline candidate

The complete readable SeqSound source is registered NonMatching and compiles through actual Ninja using repository headers only. It uses the canonical DVD_ECANCELED and SEEK_ORIGIN_BEG declarations, omits release-disabled diagnostics, and retains ordinary member initialization. No cast-based initializer, new flag, byte data, or assembly is introduced.

The whole candidate object scores 99.570816% in .text and 100% in .ctors, .data, and .sbss. Twenty-two of twenty-three retained functions score 100%; the constructor scores 94.871796%. A separate strict comparison resolves 61 relocations against the evidenced native symbols and actual SDA bases: all twenty-two other functions are byte-exact at their original addresses, all 84 initialized bytes (80 vtable bytes and four initializer bytes) are exact, and only constructor words +0x50/+0x54/+0x58/+0x5C differ. This is diagnostic placement at original addresses, not a source-selected link of this candidate.

Two destructor relocation-name mismatches were resolved by naming the outside Task destructor at 0x8028D190, size 0x40. The #802 owner confirmed its canonical name, native source proof, and ownership. Only that callee name changes here; its size, type, split, source, and code credit remain with #802.

The actual all_source and DOL build after registration exited 0 and explicit frozen verification passed. The already accepted handle's source-map/native/full-DOL audit also passed. The generated link continues to select the original SeqSound object, so this entire 1,864-byte unit earns no reconstructed-code credit until its constructor and source-selected whole link pass.

## Traps

Do not promote the initial twenty-two matching instruction streams into source credit while the constructor remains different. The entire SeqSound unit must retain its original fallback until complete object and real source-selected DOL acceptance succeed. The separate handle unit is independently accepted in [F-795-2](795-seqsound-handle.md).

Temporary header variants must be included explicitly before canonical headers; placing a modified overlay later on the include search path does not exercise its declarations. The corrected explicit-header repeat leaves five constructor differences with either flag made ordinary, five with both ordinary, and four with volatile byte flags; prepared/byte variants also change another previously exact stream. The canonical volatile bool declarations remain unchanged.
