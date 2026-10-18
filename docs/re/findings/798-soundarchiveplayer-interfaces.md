# F-798-1: Complete SoundArchivePlayer interface

## Proven layout and original interface

The original-library reference is CC0 zeldaret/ss commit 57a709b2128eb163a18e2969d561c9abb7d1fcee, src/nw4r/snd/snd_SoundArchivePlayer.cpp and its complete header. Its complete class layout is 0xEC bytes. A compiler size probe using the project header confirms that extent. The native 0x120-byte constructor at 0x80285400, destructor, setup, update and preparation methods reproduce all accessed member offsets with the complete declaration.

Existing DisposeCallback and SoundStartable bases remain at 0 and 0xC; mSoundArchive stays at 0x10. The inherited virtual order and existing public interfaces remain intact. New declarations complete the original methods, nested group/file records, two callback classes and all private members. The reference's explicitly made-up file-manager setter is omitted.

| Member | Offset | Extent |
| --- | ---: | ---: |
| mSoundArchive, mGroupTable, mFileTable, mFileManager | 0x10, 0x14, 0x18, 0x1C | 4 each |
| mSeqCallback, mWsdCallback | 0x20, 0x28 | 8 each |
| mSeqTrackAllocator, mSeqUserprocCallback, mSeqUserprocCallbackArg | 0x30, 0x34, 0x38 | 4 each |
| mSoundPlayerCount, mSoundPlayers | 0x3C, 0x40 | 4 each |
| mSeqSoundInstanceManager, mStrmSoundInstanceManager, mWaveSoundInstanceManager | 0x44, 0x6C, 0x94 | 0x28 each |
| mMmlSeqTrackAllocator | 0xBC | 0xC |
| mStrmBufferPool | 0xC8 | 0x18 |
| mMmlParser | 0xE0 | 4 |
| mSetupBufferAddress, mSetupBufferSize | 0xE4, 0xE8 | 4 each |

The original callback vtables at 0x803C6768 and 0x803C6778 pair WsdCallback's destructor at 0x80285560 with GetWaveSoundData at 0x80287B40, and SeqNoteOnCallback's destructor at 0x80285520 with NoteOn at 0x80287A10. The SoundArchivePlayer vtable starts at 0x803C6738. Complete source acceptance will resolve and compare every relocation; these header results alone earn no code credit.

## Existing inline expression

SoundActor::detail_GetActorPlayer retains the exact original reference spelling ACTOR_PLAYER_COUNT <= actorPlayerId. Reversing it to actorPlayerId >= ACTOR_PLAYER_COUNT is semantically equivalent, but Wii/1.1 folds the combined range test into an unsigned comparison. The target instead has two signed comparisons. Restoring the reference operand order adds the native missing eight bytes and reproduces all 0xA38 bytes of detail_SetupSoundImpl before relocation words are resolved. Its name, signature, result and data layout are unchanged.

## Diagnostic evidence and dependencies

The established Wii/1.1 cflags_nw4r flags reproduce all thirty-four retained instruction streams and their extents, totaling 10,172 bytes. Relocation-bearing words remain masked in this initial comparison; it is diagnostic evidence, not full source/link acceptance.

Diagnostics use existing canonical project headers and only the owners' approved pending interfaces from #851 and #859: StrmSound/StrmPlayer/TaskThread/StrmFile, SoundSystem and the complete StrmBufferPool. Those temporary scratch copies are not shipped. The complete header prerequisite depends on these interfaces reaching main, after which the actual canonical-only pipeline and existing consumers must pass before readiness.

The pre-header baseline used a fresh default directory, configure.py --map, plain Ninja and explicit frozen verification, all exit 0. SHA-256 remains ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da; scope is the main executable. Hashes of all 498 existing source objects were recorded for the post-header comparison. No source selection, split, symbol metadata or code progress changes in this prerequisite.

## Traps

Comparison operand order is material to this compiler even when the relational expressions are equivalent; retain the original reference spelling and prove consumers rather than normalizing it.

Removing a release-disabled diagnostic call from a standalone if statement must leave an empty statement or remove the complete diagnostic-only conditional. Deleting only the invocation can accidentally make the next runtime statement conditional. The diagnostic preparation preserves the empty statement. Reference warning and panic macros expand through the same disabled assert/check family when NW4R_ASSERT is unset; no substitute debug API is introduced.
