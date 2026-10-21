# Issue 152: CMsgBalloon library contracts

## F-152-1: original scope and containing unit

The original issue inventories 24 functions and 884 code bytes at
`0x800CAF00` through `0x800CB334`. They are retained library instantiations in
the existing `game/CMsgBalloon.cpp` unit, whose text extent is
`0x800C59D0` through `0x800CD728`. Issue 142 owns the original file start and
is closed; its registered source remains NonMatching. These helpers do not
establish a separate translation unit. No source-selection or issue-completion
claim follows from the producer results below.

## F-152-2: allocator ownership and inherited pair accessors

The pinned reading reference is KooShnoo/pokemon-rumble commit
`11c112f7548b26ab2cecfcf52e201b8d3b76120f`,
`libs/PowerPC_EABI_Support/MSL/MSL_C++/msl_utility`. Its MSL metadata identifies
the 1995–2006 library, but the repository contains adaptations, so its text is
not treated as an untouched copy of this target's headers. The reconstructed
changes use independently checked native calls and ordinary template APIs.

The reference `alloc_ptr` exposes `operator->()` delegating to `get()`, and
an `allocator()` accessor used by its destructor. The existing source had
mistakenly named the four-byte operator at `0x800CB030` as `get()`. The actual
`get()` is the 40-byte function at `0x800CB040`; the allocator accessor is
`0x800CB0C0`. Native list insertion calls the arrow operator, while destruction
and release call `get()` directly. Adding the reference-aware `call_traits`
typedefs avoids forming a reference to a reference when the allocator template
argument itself is a reference.

The primary pair declares `using base::second`, without another const
forwarding member. Removing the reconstructed redundant const wrapper restores
the native allocator pair calls, including the outer pair's eight-byte const
second accessor at `0x800CB1E0`.

The old wrapper also hid a different missing call in CDlgPiiSelector. Its
native list size sequence is public `size()` at `0x80095230` (36 bytes), const
size-reference accessor at `0x80095260` (four bytes), then storage second at
`0x80095270` (four bytes). A const overload of the existing `size_ref()` and
public size delegation reproduce all 44 bytes and both actual branch targets.
The prior name assigning `0x80095260` to the pair wrapper is superseded.

## F-152-3: const tree lookup and comparison contract

The same pinned repository's `MSL_C++/tree` defines const `find()` through
`do_lower_bound(key, root, end)` and a stored comparator accessor. This
matches native `0x800CB270` (196 bytes), `0x800CB9D0` (136 bytes), const root
`0x800CBA60` (36 bytes), end `0x800CBA90` (eight bytes), and comparator
`0x800CBAB0` (eight bytes). The previous reconstructed `find()` contained the
lower-bound loop itself and constructed fresh comparators.

The standard `binary_function` base of `less` also affects this compiler's
value initialization: omitting the empty base produced two extra zeroing
instructions in each native 36-byte map comparator wrapper. The standard
base and its argument/result typedefs are exposed by `functional` through a
small implementation header; that implementation filename is descriptive.

The native key comparison at `0x800CB990` is signed. An outside source copy
using `map<int, bool>` in SetupStageModels reproduces it, while the existing
unsigned-key source emits a different 28-byte comparison. The exact original
signed integer spelling is not recovered. This local source correction is
preserved outside and is not part of the header prerequisite.

## F-152-4: producer evidence and remaining acceptance work

The actual CMsgBalloon source, compiled with the canonical Wii/1.1 command
and an outside header overlay, reproduces all original 24 functions, all
884 code bytes, every code relocation, and all nine corresponding EH records
and 72 EH bytes. Each candidate symbol has one consistent native target.
The producer also makes the existing empty basic_formatter destructor visible
at its declaration; this naturally emits the otherwise missing wide-string
allocator destroy helper at `0x800CAF10`. That visibility change is outside
only: the canonical out-of-line definition in SaveData is retained.

The immutable outside archive is `152-reference/152-native24-positive`, with
input hashes, source, headers, object, original issue inventory, and proof
scripts. The broader graph explicitly records an unaccepted local weak
wstring destructor: its emitted 112 bytes differ from the native 100-byte
implementation at `0x8006BAD0`. Actual final-link binding must resolve this;
inferring a branch address is not proof that the emitted definition matches.

All 17 named CMsgBalloon class bodies retain their native extents and every
non-relocation instruction in the new producer. This is a feasibility
preflight, not full matching evidence: literal placement, every dependency
binding, generic helper coverage and whole-unit physical layout remain.

The preliminary same-base control on `7de4bb8c` rebuilt all 758 actual Ninja
source inputs. Fifty-six raw objects changed; 47 differ only in numeric local
labels after semantic normalization. The only source-selected Matching object
with a different helper inventory was CPlayer; its original functions retain
their scores and its new/removed helpers are discarded at link. A real fresh
link and frozen verification passed with an identical DOL. Every NonMatching
score delta is recorded separately in the outside consumer receipt; this is
not a blanket assertion that provisional object code was unchanged. Final
publication repeats controls after each official landing rebase.

The complete current-base check at `aa756c75e4c0` rebuilt 759 source objects,
including 710 source-selected objects. Forty-seven raw differences normalize
to numeric labels only. CPlayer's original native function scores and all
strong function bodies and call targets are unchanged; only discarded helper
inventory differs. Linked ELF allocations, logical symbols after the four
documented renames, and the entire frozen DOL are identical. The final list
size chain is exact in the real CDlgPiiSelector producer.

NonMatching deltas are intentionally reported separately. The Balloon
four-byte lookup forwarder changes its relocation name to the const lookup;
its score changes from 100 to 95 solely for that name, while the native
lookup branch is independently resolved in the 24-function proof. Its old
non-const lookup symbol no longer describes the emitted function. Provisional
MapPiiTable InsertString changes from 95.91235 to 87.84064 and InsertWString
from 95.92857 to 91.62698 under the corrected lookup hierarchy; neither was
previously instruction-exact. No preservation claim is made for those
unaccepted provisional instruction streams. The complete per-function
before/after records are in the immutable per-head consumer receipts outside
the repository. No unit status changes in this prerequisite.

## F-152-5: Original primary container and string producer controls

The recovered unadapted 2006 MSL headers provide direct source evidence for
the parent pointer's `__pointer_plus_bit` type, iterator increment through a
pointer reference, the list pointer-to-iterator factory, and the list
allocator-taking sentinel construction. Outside adaptations reproduce the
corresponding native helpers without changing compiler flags. The list
constructor is exactly 100 bytes. Replacing the tree destruction routine's
raw delete with the primary `node_alloc().deallocate(pNode, 1)` reproduces
its complete 128-byte instruction stream and call targets, versus the
previous 116-byte reconstruction.

The current combined outside producer maps 309 reachable functions, of
which 301 have complete native instruction streams after resolving call
targets. The 17 game methods remain exact at 8,688 code bytes. All 24
original issue functions and their nine EH records, totaling 72 bytes,
remain independently exact. These are producer results, not a Matching
unit claim: literal section placement and eight first-stop helper gaps
still prevent complete acceptance. The recursive producer does not replace
the full object, EH, data, and frozen-link gates.

The primary string adaptation supplied by the Sound investigation restores
the native assignment tail branch and 76-byte string append. The formatter
parser's character insertion uses `mResult += c`, matching the primary
84-byte character insertion method. Primary `push_back` itself delegates
to that operator; it does not delegate to the pointer-length append API.
The remaining string assignment implementation is 184 bytes versus native
180, so no canonical string change is accepted here.

Reproducible controls and receipts are outside the repository at
`152-source-context.py`, `152-control-primary-string.py`, and
`152-reference/primary-string-list-consumer/recursive.json`. The original
24-function and EH receipts are in
`152-reference/primary-list-constructor/primary-code-graph.json` and
`primary-own-eh-proof.json`.

## F-152-6: Original tree algorithm visibility and size roles

The original public tree also forwards `begin`, `end`, and `find` directly
to storage, and set insertion calls its `insert_one` API. Independent
MapPiiTable producers reproduce all 29 instruction streams with these
roles. The compatible adaptation retains the older middle-layer APIs and
exposes inherited backend types and methods through using declarations.
Balloon's four-byte public forwarders at 800C94D0, 800C9520, 800C95B0,
and 800C9760 are named from that primary class role and actual pipeline
symbols. Current-consumer and linked acceptance remain separate gates.

The same-base preflight at `2a8c4aa398e1` covers 760 source objects and
713 source-selected Matching objects. All previously exact streams retain
their bytes and semantic relocations; Matching strong functions and linked
allocated ELF/DOL remain identical. CWaza changes only unused weak helper
inventory and generated local labels. NonMatching BWorld and MapPiiTable
scores improve, while NPpuVM Init remains nonmatching and exchanges four
stack-byte references between offsets 12 and 13 without changing its
608-byte extent. Its score changes from 99.86842 to 99.84210; this provisional
stream is not presented as preserved or corrected. The native insert and
its 24-byte EH record, original 24-function producer and 17 game bodies
remain independently exact. Receipts are under
`152-reference/header-verified-2a8c4aa398e1`.

The recovered original header distinguishes the public `__tree::size`
method from `__tree_deleter::size`. The existing reconstructed `tree`,
`red_black_tree`, and `tree_imp` already represent the public, constructor,
and storage roles, but the public size method was inherited rather than
defined. A public size method returning the base const `sz()` restores
the distinct 36-byte functions at `0x800C95E0` and `0x800CCEE0` without
changing any object layout. The mutable `sz()` call also restores the
separate four-byte increment accessor at `0x800CCF20`.

The native insert body at `0x800CC7A0` is 392 bytes. The primary maximum-size
check, direct length-error construction, node allocator calls, allocation
owner arrow, and `sz()` increment reproduce all code and relocation targets.
Its 24-byte EH record initially retained an extra protected interval after
the allocation owner released its node. The missing information was the
balance algorithm: the reconstructed header declared only an external
helper, while the original header supplies its complete template member
definition. Making the original algorithm visible restores the exact EH
interval and destructor target.

An explicitly inline free-function adaptation changes MapPiiTable inlining
and is rejected. An ordinary non-inline free definition in a shared header
would violate the one-definition rule and remains diagnostic only. The
coherent reconstruction uses the original empty `std::__red_black_tree`
algorithm base, ordinary out-of-class template definitions, and the original
color enumeration and accessors. Existing compact and full tree storage
classes inherit that empty algorithm base. No compiler flags, attributes,
or forced retention are used.

The complete producer proof is archived at
`152-reference/152-primary-insert392-eh24-positive`. An independent
MapPiiTable producer confirms four helpers totaling 1,324 code bytes and
both 96-byte insertion EH records, including all relocation and cleanup
targets. Its receipts are `208-reference/208-tree-helper-proof.json`.

The visible balance body is an external weak definition and is not claimed
native-exact. The source-selected link must retain the existing strong
900-byte implementation at `0x80059AF0` from the explicitly foreign
`build/wpse-v1/obj/game/BModelObj.o`. Each acceptance round regenerates the
MAP, checks that module in the Ninja link inputs, and checks the strong ELF
symbol and extent. It also repeats the complete current-consumer and frozen
ELF/DOL controls before publication.

The primary reference is `tree`, revision 1.36.2.3, dated 2006-02-09,
in the recovered `alpha2006/CW_RVL/PowerPC_EABI_Support/MSL/` tree.
Its notice is preserved here because source comments are prohibited:
“Copyright © 1995-2006 Freescale Corporation. All rights reserved.”
The reference attributes the insertion and removal algorithms to Cormen,
Leiserson, and Rivest, *Introduction to Algorithms* (MIT Press, 1990).

## F-152-7: Complete Balloon instruction and exception producer

The post-1203 outside producer covers all 436 original functions from
800C59D0 through 800CD728, totaling 29,012 instruction bytes. Every code
relocation is resolved consistently, and all 226 exception records totaling
3,664 bytes match with their actual cleanup targets. There is one source
function per native address, no instruction differences, no unresolved
targets, and no conflicting symbol bindings. Twenty reachable RTTI/vtable
objects also match individually. The immutable source, overlay, object,
compiler command, audit scripts and SHA-256 manifest are at
`152-reference/152-native436-unique-positive`.

This is not Matching acceptance. The object still fails section placement
constraints. Defined weak functions outside this unit, including the
visible formatter constructor, are not claimed reconstructed and require
explicit foreign fallback and dead-stripping proof in the real source link.
The shared header adaptations have not passed current-consumer controls.

The remaining instruction gaps resolve through original declaration and
class roles: the list default constructor initializes its size and node
allocator separately; public list begin/end expose storage methods rather
than duplicating them; map iterator assignment is implicit; vector cleanup
uses allocator accessors and natural decrement expressions; split-buffer
loops use the original pointer expressions; expansion carries its capacity
output; append reallocation uses begin/end; and clear uses the one-argument
erase API. String assignment needs the original reference-returning
long_size and unsigned-char short_size accessors, ordinary out-of-class
assignment/append forwarding, and mutable-pointer dispatch through the
const-pointer implementation. The formatter constructor body already
present in SaveData.cpp supplies genuine visibility for retained string
constructor and allocator cleanup helpers.

The audit starts from actual strong game functions, follows real code,
vtable and exception relocations, and separately checks the retained
primary string constructor and allocator destructor. Seeding every old
template name from symbols.txt instead creates false conflicts because
several provisional names assign the wrong vector specialization to an
otherwise identical accessor. Those names are not treated as independent
evidence.

## F-152-8: List construction and iterator storage contracts

The original list default constructor initializes `size_` from a size value,
initializes its node allocator from `alloc()`, and chains the two sentinel
links. Its ordinary out-of-class inline definition reproduces Balloon's
100-byte constructor and eight-byte exception record. This exercises the
missing second-value constructor in compressed-pair storage mode 1; the
primary constructor initializes only its stored second value.

The original public list exposes storage begin/end with using declarations.
Retaining independent public implementations creates duplicate functions
that individually fit the same native address. Moving the const iterator
accessors to storage and exposing both overloads eliminates those aliases.
The four native list accessor identities at 800C6D80, 800C6D90, 800C6F50,
and 800C6FA0 follow the actual one-to-one producer graph. Existing legacy
sentinel accessors remain available. Map iterator copy assignment is
compiler-generated in the primary interface; removing its invented body
restores the native 12-byte assignment while preserving the API.

These bounded contracts are separated from the outside string and vector
changes. The complete same-base gate at `f8ca25936f01` passes 762 source
objects, 714 Matching objects and all 321 previously exact functions in
affected objects. Matching strong functions, linked allocated ELF and the
frozen DOL remain identical. NonMatching CDividedGround retains all scores;
Balloon's constructor, destructor and SetupStageModels scores improve.
The 24-function/EH72 and 17-game-body producers remain exact.

The native storage-role corrections also preserve the existing CFileData,
CPiiPersonalData and CLayout motion-list interfaces. The CPiiPersonalData
nonconst begin/end functions retain their foreign strong 12-byte bodies
from CDlgPiiList.o; the const 52/48-byte functions remain supplied by
CDlgPiiSelector.o. Every gate checks their fresh MAP rows, Ninja inputs,
strong binding, addresses and extents. Their emitted weak alternatives are
not claimed native-exact. The original full-header producer independently
identifies C6EC0 as the four-byte node-allocator copy constructor and C9710
as transformed-map iterator assignment, correcting earlier labels without
changing native code.

## F-152-9: Primary vector storage and split-buffer algorithms

The original 2006 vector places begin/end in its storage/deleter layer,
uses allocator/capacity accessors, and destroys the preceding element with
alloc().destroy(--p). The split buffer computes its live range directly
from data/start/size and updates size in the construction-loop increment.
The append-reallocation definition is ordinary out-of-class. Restoring this
coherent primary context reproduces the Balloon erase, construction,
destruction, growth-dispatch and append-reallocation helpers.

The immutable outside producer at
`152-reference/152-native436-unique-positive` retains 436 functions /
29,012 instruction bytes and 226 EH records / 3,664 bytes exact. It includes
other pending source/string changes and is not Matching. Independent Map
receipts at `152-reference/152-map-vector-independent` preserve all 29
functions / 15,184 bytes and all 19 EH records / 500 bytes with applied
relocation targets.

The same-base control at `7b46a1163eda1a8911f85b2bcf74d128d1e7fdc9`
rebuilds 762 source objects, including 714 Matching units. All 983 affected
previously exact streams retain their bytes and semantic relocations;
Matching strong definitions, allocated linked ELF and frozen DOL are
identical. NonMatching native score changes only improve BWorld,
CGloballyReusableTexBuf, CMsgBalloon and SaveData; other affected provisional
units retain their scores. CPpuRegister retains every Matching strong
function. No unit status changes.

Five provisional names put native storage accessors in the constructor
layer: 800C62E0, 800C7240, 800C7350, 8010C360 and 8010C370. Primary inheritance
and actual typed callers prove the __vec_deleter identities. Their raw
native bytes remain identical. The guard normalizes these baseline names
before comparing every previously exact stream; it skips no stream.

A separate inside-class public push_back placement experiment moved the
first ordering mismatch from index 12 to 22, but changed formatter parse
from native 424 bytes to 420. The unchanged control still reproduces all
436 functions. That experiment is rejected and excluded. Full Balloon
physical placement and source-only provenance acceptance remain pending.

## F-152-10: Borrowed interfaces and actual link bindings

The complete producer's resolved instruction streams do not by themselves
prove final symbol selection. A real link with the complete outside object
exposed borrowed helpers still named fn_ or dtor_ in original objects.
That allowed additional weak code/EH to survive. Naming the evidenced
wstring representation/accessor, formatter, shared-owner and operator
interfaces leaves the original fallback definitions in use. No borrowed
implementation is claimed reconstructed. All original DOL bytes remain
unchanged in the metadata-only configure/Ninja/frozen control.

The native 800968D0 routine is a 116-byte position-transform wrapper that
calls MTX34/VEC3 conversion operators and PSMTXMultVec, returning the output
pointer. It cannot share the existing strong SDK VEC3Transform identity at
8023F120, whose body is 60 bytes. Its original spelling is unknown. The
external declaration TransformPoint is descriptive, not a recovered name;
it adds no body or forwarding shim. The actual Balloon caller with this
declaration preserves all 436 isolated instruction streams and 226 EH
records. Final acceptance also requires the fresh MAP and Ninja graph to
select the original CDlgPiiSelector object and its strong 116-byte function
at 800968D0.

A diagnostic input-list link is not a source-selected Matching result.
Its complete 436 linked functions retain their native sizes, but native
physical order remains wrong. The pending genuine non-template do_replace
binding is coordinated with the string prerequisite and excluded from this
proposal. The legacy external string interface is preserved meanwhile.

Renaming an automatic fallback changes its generated object-file name.
Fourteen STT_FILE entries changed from auto_fn/dtor names to recovered
names while every allocated ELF byte and the DOL stayed identical. The
consumer guard pairs only those file identities using fresh before/after
MAP rows at the same native address and original Ninja input paths. It
still compares the full symbol multiset, all allocated ELF sections, named
previously exact streams, and Matching strong definitions. No instruction,
relocation, function symbol or unnamed file record is exempted.

The reviewed-head preflight at
`4e9157598370645abd5f444eb84c502fccd81b00` passes all 764 current source
objects and 716 Matching units. No source object has a semantic delta;
only numeric local identifiers differ. Allocated ELF, normalized complete
symbol identities and the frozen DOL are identical. Nineteen generated
module identity pairs are individually recorded, including the fourteen
STT_FILE differences noted above. The complete 436/226-EH caller audit and
fresh strong 116-byte TransformPoint fallback proof both pass. These guards
run again after every official landing rebase.

## Traps

- A flattened container reconstruction can produce each native helper
  separately while merging two real class identities. The original tree
  insert body reaches its native 392 bytes with the primary inline throw,
  maximum-size check, node allocator, and allocation-owner arrow. However,
  the reconstructed `tree_imp::size` then binds to both `0x800C95E0` and
  `0x800CCEE0`. Original headers distinguish derived `__tree::size` and
  base `__tree_deleter::size`, each 36 bytes. The binding conflict rejects
  this incomplete adaptation. F-152-6 resolves it using the existing real
  class roles and their primary public size method, rather than inventing
  an additional storage class or helper.
  Reproducer: `152-primary-insert-context.py`; receipt:
  `152-reference/primary-insert-context/recursive.json`.

- Native instruction proofs for an owned range do not prove every emitted
  external weak definition. Extending the original-header oracle to all
  reachable defined functions audits 521 functions and exposes external
  weak bodies with different extents. Actual source selection must prove
  which external strong definitions win and which weak bodies are stripped.
  The 390-function owned-range oracle remains diagnostic rather than whole
  source acceptance. Receipt:
  `152-reference/full-primary-external-defined.json`.

The bounded follow-up candidate changes only the tree parent pointer type,
stored comparator use, reference-based iterator advancement, allocator-based
node destruction, and the list pointer-to-iterator factory. Existing public
APIs remain available. It excludes the unfinished insert-node, vector, and
string adaptations. Initial all-source compilation and frozen-link checks
pass. The same-base control on head `f1b25da98220` also passes all 759 actual
Ninja source inputs and 710 source-selected units, with identical allocated
ELF sections, semantic linked symbols after the four evidenced renames, and
frozen DOL bytes. CPlayer changes only dead-stripped weak helper inventory;
all strong functions and every native function score remain unchanged.
The only other semantic object delta is provisional CMsgBalloon: its
iterator increment and list insertion reach 100%, and find-or-insert
improves from 97.86957% to 99.347824%. No previously 100% native stream
regresses. The exact 24-function/72-byte EH proof and separate 17-method
producer are rerun with the current canonical headers.

- The coherent original-header control of the complete Balloon source
  compiles with the original MSL math header forwarded through the existing
  `MSL_C/math.h` include path and the exception override declared `throw()`.
  Mixing the reconstructed math wrapper with original headers instead
  fails on a duplicate `INFINITY` macro. No macro suppression is needed.
  The original-header producer reproduces 390 reachable functions and
  25,512 instruction bytes with resolved targets, but still fails 100
  literal section placement constraints. This is not a full-unit or
  all-original-issue acceptance result. Its immutable input/object/receipt
  archive is `152-reference/152-primary390-positive`.

- A four-byte native forwarder can belong to a caller API rather than an
  invented wrapper in its callee's class. The list size chain above preserves
  the native extra call while removing the incorrect generic pair wrapper.
- A same-size helper is not a sufficient type proof. The original unsigned
  map key inference generated an unsigned comparison where the native code
  performs a signed comparison.
- An ELF NOBITS section has no file payload. Comparing file bytes at its
  nominal offset reads unrelated later tables and falsely reports changed
  BSS contents. Compare its allocation, flags, alignment and extent instead.

- Balloon source visibility controls preserve no new placement acceptance.
  Moving its unchanged constructor into the class header keeps all 436
  instruction streams and 226 EH records exact, but emits the constructor
  group before Init; the first physical mismatch becomes index 4. Keep the
  late constructor definition in the positive checkpoint. The receipt is
  `152-reference/order-inline-header-constructor/order-proof.json`.
- For Balloon, making late methods ordinary out-of-class definitions in
  native address order changes Calc literal pooling: one candidate symbol
  would need two different native addresses. The consistent-binding audit
  rejects this context before any link claim. See
  `152-reference/order-native-method-definitions/audit.log`.


### Real source-selected pipeline diagnostic after PR1222

The complete Balloon source was selected locally through `Object(Matching, "game/CMsgBalloon.cpp")` solely as a diagnostic. The real configure, compiler and linker pipeline completed; the frozen checksum rejected the fresh DOL. This does not establish Matching acceptance. The actual pipeline object independently preserves all 436 function streams and all 226 EH records, with every paired target checked. A fresh `all_source` rebuild and comparison of all 717 previously Matching objects preserves allocated bytes, section metadata, semantic relocations and symbol attributes; 28 objects differ only in compiler numeric labels.

Relative to the verified baseline, the resulting ELF grows by 8 extab bytes, 12 extabindex bytes, 80 text bytes, 64 data bytes and 16 sdata bytes. The retained local 68-byte free vector length-error helper is an additional body; mapping it to an unrelated native role is not accepted. The fresh MAP also shows owned function permutations, so removing the extra helper alone would not prove whole-unit layout. Evidence is preserved outside in `152-reference/full-source-pipeline`: actual compiler object, ELF/DOL/MAP, complete native proof, section deltas, owned function layout and Matching-object control.

The first link attempt exposed stale source-object timestamps inherited through `copy2`; explicitly refreshing the changed source caused a real recompile and resolved the apparent missing wide-string constructor, allocator destroy and integer comparator. Future source-selected diagnostics must confirm an actual compiler edge ran before interpreting unresolved helper identities.


### Primary vector error owner and inherited interface

The original 2006 MSL vector header declares `__vector_common<bool>` with an ordinary out-of-class static `throw_length_error` definition; `__vec_deleter` publicly inherits `__vector_common<true>`. Adapting this exact role removes the reconstructed file-local free error routine. The original native error routine at 80088550 retains its existing 68-byte extent and receives the actual primary template identity. No exception algorithm, source flags or expected hashes change.

The complete Balloon diagnostic retains all 436 code streams and 226 EH records with this inherited primary contract. Its actual linked text, extab and extabindex now have exact native addresses and sizes. Its remaining data growth and function ordering still prevent acceptance. Isolating only this bounded header/interface prerequisite, with Balloon left NonMatching, passes the real all-source build, full link and frozen DOL. Two accepted consumer objects reorder emitted weak helpers without changing their function instructions; the verified isolated final allocated ELF is byte-identical to the baseline. This differs from the previously rejected free/global-helper variant and does not authorize that variant.


### Original shared IDlg RTTI removes all remaining section growth

The native eight-byte RTTI record at 804B5FF0 points to the unique `IDlg` literal at 804B5FE8 and the existing 20-byte inheritance chain at 8039C420. Naming that original record `__RTTI__4IDlg` lets the linker coalesce the duplicate Balloon RTTI and chain. The real source-selected Balloon link then has exact native addresses and sizes for every allocated section, including text, EH, data and SDA. Its frozen checksum still fails because owned function/literal placement is not yet native. The actual object still proves 436 functions and 226 EH records exactly. Root's independent data scan is preserved in `346-reference/346-balloon-data-identity.json`; the complete linked diagnostic is in `152-reference/balloon-idlg-binding`.

The existing `#pragma dont_inline on` originates in the first partial Balloon source commit 8ee14d478 (#996), whose findings explicitly distinguish Balloon's non-inlining behavior from MoneyLife. Removing it from the current actual source collapses the reachable native graph and expands the factory from 140 to 1528 bytes, IsStageCleared from 116 to 176 and GetStageClearCount from 176 to 224. This is direct evidence for the existing unit's outline boundaries, not permission to guess unrelated compiler settings.

A bounded ordinary-method `-str noreuse` diagnostic follows the compiler's documented option and the native duplicate `below_site` literal endpoints. It resolves the old pooling assertion but changes Calc 2348 to 2340, SetupStageModels 1804 to 1796 and formatter parse 424 to 420, and introduces inconsistent destructor/formatter call identities. It is rejected. The old ordinary-method failure was a real single-symbol/two-native-endpoint conflict, not a stale section-base inference: source `below_site` at relocation pairs 626/62A and 70E/712 in Calc had native endpoints differing by 24 bytes. Receipts are under `152-reference/ordinary-noreuse`, `no-pragma-current` and `ordinary-no-pragma-noreuse`.


### Coherent primary definition context restores retained clusters

The following outside changes preserve all 436 native code streams and 226 EH records while correcting real retained layout. They remain unpublished until the fresh complete consumer gate passes.

- Reorder only the derived overriding method declarations to the reverse of observed deferred native emission. Inherited virtual slots are unchanged. This restores the native relative order of all game method bodies.
- Restore primary public vector push-back definition visibility inside the class, preserving its existing POD/non-POD body. Together with the corrected derived declaration context, the previous formatter parse regression disappears. Tag-specific sub-push implementations move unchanged to ordinary out-of-class definitions, matching primary vector 1309/1364. This restores Draw, CalcView, CalcMtx, Calc, UpdateBalloon and SetupBattleWarp to their exact native addresses.
- Make the natural formatter `c_str` getter body visible. Its selected 784-byte foreign implementation remains fallback, not reconstructed source coverage. The visibility restores the entire native str/parse helper cluster at 8460 through 8750. Its foreign body still requires an independent source proof before publication.
- Move the unchanged character append operator to its actual ordinary out-of-class primary string definition context (1550), removing two prematurely emitted helpers and restoring SetupStageWarp and SetupStageModels to their exact native addresses.
- Restore ordinary out-of-class tree storage destructor, keyed find and keyed find-or-insert definitions (primary tree 845, 1375, 1019), preserving their bodies and APIs. Restore ordinary vector count append and actual tail erase definitions using their primary algorithms and roles.
- Iterator increment now calls the existing primary algorithm base, through its derived alias and the explicit nested-iterator friendship required by the original header. The native increment at A760 and leftmost at BB90 receive those original algorithm member identities. The old free APIs remain available. No scope, size or type is changed.

The coherent snapshot `152-reference/152-coherent-436-before-consumer-control` contains all 201 actual dependency headers, source, reproducible compiler command, strict 436/226 proof, linked ELF/DOL/MAP, full worktree patch and SHA256 manifest. The actual link currently has 134 functions at native addresses, no retained nonowned functions and no owned functions coalescing outside the unit. Its frozen checksum still rejects remaining placement; no issue closure is claimed.

The constructor's remaining 16-byte shift is the premature owned shared-pointer arrow accessor (native 70B0), not extra code or foreign coalescing. Both placing the constructor body in the class and placing its unchanged outside inline body immediately before the factory move the entire constructor chain ahead of Init/Show and are rejected. The existing late constructor context remains the positive control. Moving the unused legacy erase-at-end body outside had no retained effect and was discarded; the actual resize path is its tail-erase compatibility role.

The fresh 8b07def40f object preserves all 436 instruction streams and 226 EH records, including the newly accepted scalar constructor layers. The isolated three-header control restores canonical Balloon source and config while checking string, vector and tree independently. Its first run passes the frozen executable but correctly rejects old exact iterator streams when the two proven algorithm-member symbol renames are omitted. Header role changes and corresponding native identity metadata must be controlled together; removing the naming evidence is not an instruction regression. The role-bound rerun and exact logs are preserved under `152-reference/context-prereq-after-rebase`.

The role-bound isolated gate passes on 8b07def40f: 767 actual source objects, 717 Matching objects, every prior exact stream retained, full allocated ELF and normalized symbols identical, and frozen DOL identical. Nineteen object emission contexts differ; six provisional function scores change. Notice Init and SaveData FormatWide remain imperfect and change scheduling/context slightly; neither was previously exact. Pause push/screenshot and Title Refresh improve, and NPpu ScriptBinding_String reaches native100. No NonMatching file is declared complete from these scores. A separate archived actual coherent producer recompiled with exactly the three proposed headers retains 436 code streams and 226 EH records with all semantic targets. The official per-rebase guard is `152-land-context-prereq.py`; proof receipts remain outside under `152-reference/context-consumer-rolebound-8b07def40fc9`.
