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

## Traps

- A four-byte native forwarder can belong to a caller API rather than an
  invented wrapper in its callee's class. The list size chain above preserves
  the native extra call while removing the incorrect generic pair wrapper.
- A same-size helper is not a sufficient type proof. The original unsigned
  map key inference generated an unsigned comparison where the native code
  performs a signed comparison.
- An ELF NOBITS section has no file payload. Comparing file bytes at its
  nominal offset reads unrelated later tables and falsely reports changed
  BSS contents. Compare its allocation, flags, alignment and extent instead.
