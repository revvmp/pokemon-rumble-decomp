# F-795-2: Sequence sound handle retained unit

## Boundary and ownership

Issue #795 owns SeqSoundHandle::DetachSound at 0x80280F60, size 0x38. The preceding SeqSound initializer ends at 0x80280F5C, and Sound3D actor code follows at 0x80280FA0. The pinned sequence-file order and matching original method establish a separate snd_SeqSoundHandle.cpp translation unit. Its retained extent is .text 0x80280F60 through 0x80280F98, leaving the following alignment gap to the next automatic object. It has no retained initialized data, BSS, or initializer contribution.

The reference is the same pinned CC0 original library source identified in [F-795-1](795-seqsound-interfaces.md). The reconstructed method clears the attached sound's temporary-handle pointer only when it refers to this handle, then detaches its own sound pointer. The unretained reference constructor earns no code credit and is omitted from this retained unit.

## Split baseline

The split-only configure and actual custom-directory DOL build exited 0, and explicit frozen verification of build/795-header-baseline/wpse-v1 passed. This establishes only split byte identity; source-object and source-selected whole-DOL acceptance remain separate gates.

## Traps

The SeqSound constructor fidelity problem recorded in F-795-1 does not affect this independently retained handle unit. Completing the handle unit does not complete issue #795 or establish matching for the SeqSound translation unit.
