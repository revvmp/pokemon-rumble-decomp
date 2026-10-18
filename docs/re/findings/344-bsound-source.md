# F-344-3: BSoundSource native diagnostic

This is an outside diagnostic checkpoint, with no source selection or completion credit. The boundary before BSoundSource remains unproven: the first owned function is801E8740, while the preceding SoundWork unit belongs to343. No neighboring split, header or source has been taken over.

RTTI at804B5A70 points to the original BSoundSource name at803996F8, with no base list. Its vtable starts803C2D54. The observed layout is SoundHandle at0, shared_ptr<Sound3DActor> at4/8, VEC3 atC/10/14, and the class vptr at18, for size1C. Existing SoundActorParam.pitch is at4C; volume is at48. The methods at801E8A80 and801E8BD0 are therefore SetPitch and GetPitch.

An independently written ordinary class and TR1 ownership diagnostic uses the existing Wii/1.1 game command unchanged. Eleven of thirteen original344 functions,956 instruction bytes, compare exactly after resolving actual relocation targets. Both remaining methods have their exact original extents: StartSound320 and SetPitch324. Each differs in only three stack-slot instruction words: the diagnostic places a zero-valued constructor temporary at8 and the owning guard atC; the target usesC and8 respectively. All47 text relocation sites across the thirteen functions resolve to the original targets. The strict audit preserves all non-relocated instruction bits and records those six remaining differences explicitly.

The seven class methods are constructor32, destructor264, StartSound320, StopSound32, SetPosition104, SetPitch324 and GetPitch36 bytes. The six retained ownership helpers are NonCopyable destructor64, get_deleter76, dispose36, shared_ptr destructor132, move_ptr destructor116 and control destructor64 bytes. Other emitted weak helpers are excluded from owned credit. Exception tables and complete allocated data have not yet been accepted for this diagnostic.

Declaration-only reference inspection establishes std::move_ptr storage as compressed_pair<T*,D>, its private nat aggregate containing int dummy_, and its templated raw-pointer constructor with a default nat parameter. The declaration facts do not establish an implementation body. The independently authored holder and shared-pointer constructors remain hypotheses. The source uses an ordinary scope guard that destroys the actor if control allocation throws and releases that guard after successful control construction.

Wii/1.3 with the same game flags reproduces the same three differences in each method. Ordinary explicit-deleter, copy-initialization, out-of-class constructor and control-initialization variants do not resolve them. No flag, forced register, artificial padding, manual stack slot or dummy helper has been accepted. The current first-only compressed-pair constructor is outside-only and is not part of the separately proposed canonical header.

## Traps

- A GetVolume interpretation almost matches GetPitch but selects the wrong proven SoundActorParam field. Preserve the existing canonical parameter layout and identify the operation from its actual4C access.
- Non-relocation equality alone is insufficient. The final diagnostic separately resolves47 original call, vtable, RTTI and constant targets; only its explicitly recorded stack-slot differences remain. This still earns no Matching or issue-completion credit.
