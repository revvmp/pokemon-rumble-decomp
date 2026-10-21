# F-264-1: actual ScriptPii head bindings

Original issue #264 owns 27 functions and 1,900 bytes at 0x80153930–0x80154134. The actual source unit is shared with first-owner #262 and neighboring #263; no competing split or header is proposed here.

Six actual script bindings at 0x80153D50, 0x80153DA0, 0x80153E60, 0x80153F10, 0x80153F70 and 0x80154030 implement head-facing operations and the player index query. Their original binding-table names are retained. The head direction APIs forward to CPii native methods 0x800DF630/680/6F0/7D0, with VEC3 coordinates or typed object/node arguments. The player getter reads the CPlayer pointer at +0x14C, followed by the accepted CPlayer::GetPlayerNo accessor.

The native 60-byte engine helper at 0x80153FF0 pops one Slice and passes it to CEngine::ToString. Its descriptive PopString name is inferred from that operation, while its owner and signature follow the actual two native member calls. No string scheduling workaround is used.

The immutable actual producer and independent auditor are in `C:/MassStorage/GitHub/264-reference/264-strict-ten-20261021/`, with a SHA-256 manifest. The six bindings plus PopString, the player getter, typed cast, and type predicate prove ten original functions, 960 bytes, all 40 resolved calls, and nine exception records/72 bytes. Every relocated instruction is compared in full. This is outside native evidence, not source-selected acceptance or completion.

The other seventeen original functions, 940 bytes, are natural Enemy/Boss ownership helpers emitted by #263 factories in the containing source unit. The coordinated #263 owner independently reported its full 45-function factory graph and 31 exception records exact. Acceptance still requires one complete actual source producer, all original inventory identities, native metadata/data placement, and the frozen source-selected link.

## Traps

Calling every inherited VM operation through the derived CPpuEngine local changes register ownership in the head-facing bindings. Calling inherited PopFloat/PopString/PushNull through the base formal, while using the derived local for PopWorldObj, reproduces all native streams. GetPlayerIndex retains the derived local for its final PushInt. This is a measured declaration-context result for these actual functions, not a general register-allocation rule.

The outlined inlining policy belongs to the complete #262 producer and remains under whole-unit validation; no isolated production pragma or compiler flag is published here.

## Final checkpoint

Independent replay of the neighboring factory object's seventeen owned functions proves all 940 code bytes and 28 resolved destinations. Its eleven associated exception records, 104 bytes, also compare exactly after resolving cleanup destinations. Combined with the ten-function producer above, all 27 original functions/1,900 bytes, 68 resolved code destinations, and twenty exception records/176 bytes have native evidence across two real fragments. This does not establish a complete source-selected translation unit.

The readable ordinary source fragment, additive header proposal as a patch, and strict native receipts are preserved under [the diagnostic checkpoint](../checkpoints/264/ScriptPii-fragment.cpp). They are diagnostic material, not registered game source or accepted shared declarations. The complete object/source/header/auditor archive remains `C:/MassStorage/GitHub/264-reference/264-final-checkpoint-20261021/`, sealed by its SHA-256 manifest. It includes the separate actual factory producer and its native proofs so the leading helper evidence is reproducible.

The user ended the parallel run before combined-TU integration. No further #265–#267 bodies were reconstructed. The claim is released without a completed issue or progress increment. Resumption must integrate the six ordinary bodies with first-owner #262, combine header additions with #263's independent adapters, replay all current native controls, and establish exact metadata placement/source-only provenance/frozen DOL before any whole-issue completion.
