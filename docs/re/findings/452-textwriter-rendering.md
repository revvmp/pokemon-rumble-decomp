# F-452-1: TextWriter rendering and source ownership

Issue #452 retains 56 original functions and 21,692 instruction bytes from 0x8029C320 through 0x802A1944. These belong to the complete TextWriterBase unit beginning at 0x8029A790, first owned by #451. The #451 owner approved independent investigation of these bodies while retaining the source file, header, split and configuration entry. No production source or metadata is changed by this finding. CharWriter belongs to #450 and remains independently attributed.

## Reference and independent reproduction

The source reference is the CC0 zeldaret/ss revision 787b0b1e9d30e3c1d91ced61350b059ffc5a2411, with the file digests and license inspection recorded in F-451-1, currently published in [PR #898](https://github.com/revvmp/pokemon-rumble-decomp/pull/898). An independent external copy of that owner's diagnostic generator, inputs and audit reproduces 71 of the complete unit's 81 retained functions, covering 17,712 native instruction bytes by exact extent and every non-relocation instruction word. This total spans both issues and is not new source-linked credit. Call, address, data, unwind and full source-selected acceptance remain necessary.

The normal char and wide PrintImpl bodies have the correct extents, 0xA58 and 0xA7C, but each still differs in 306 non-relocation words. The candidate stack frame is 0x310 rather than the native 0x300. Its Glyph and GXTexObj locals occupy different locations, and many corresponding live values use different registers. Matching sizes do not establish identity.

The display-list implementation remains structurally incomplete. The ordinary candidate retains an oversized 0xD48 CharWriter rendering helper outside the two text loops, leaving those loops at 0x58C and 0x5B0 instead of the native 0x8B0 and 0x8D4. The native display-list primitive helpers and the more restricted inlining pattern require further evidence.

The isolated baseline inherits main ea4ffb474229f4c66f35b85f42af48631c1de1b1. Its complete default-path configure --map, 539-target Ninja build and explicit frozen main.dol verification exit zero. The diagnostic source remains external and unregistered; this baseline verifies the existing project rather than accepting any TextWriter reconstruction.

## Traps

- Passing the baseline-adjusted glyph position directly as arguments, keeping only one coordinate local, or constructing a const position vector does not reproduce the normal rendering bodies. The four scalar forms retain the 0x310 frame and increase the instruction differences; the vector form grows the body to 0xA64. These variants are diagnostic and were restored.
- Moving helper definitions after the template implementation does not resolve the normal frame or display-list outlining differences. Changing the three GD primitive helpers between static inline, inline and external definitions likewise leaves the oversized rendering helper. Do not choose linkage solely to force inlining.
- Disabling IPA emits the expected primitive-helper sizes but shrinks the normal bodies to 0x838/0x85C and the display-list bodies to 0x698/0x6BC. Disabling automatic inlining instead yields normal bodies of 0x854/0x878 and display-list bodies of 0x87C/0x8A0. Neither control justifies a production flag change.
- The initial custom-path baseline failed at CHECK because the existing SHA-1 input still named build/wpse-v1/main.dol, which did not exist in this new worktree. Returning to the default build path, completing Ninja and independently verifying that actual output resolved the failure. No expected hash, frozen manifest or reference artifact changed.

No function or byte from this investigation is marked Matching. All compiler-option and helper-layout experiments remain outside the repository.
