# F-315-5: Original CDisablePadRumble identity and empty-class ABI

## Native identity and size

The constructor at `0x801B4840` and destructor at `0x801B4BF0` belong to the original class `CDisablePadRumble`. This is established independently of their inferred behavior:

- Factory `0x8012C660` loads allocation size one at `0x8012C680`, calls scalar new at `0x8012C684`, then calls `0x801B4840` on the result at `0x8012C694`.
- It passes that object to shared-pointer control construction at `0x8012EA20`.
- The sixteen-byte control's vtable is `0x803B56BC`. Its RTTI at `0x804B78F0` points to the string at `0x803B56D0`: `std::tr1::detail::shared_ptr_deleter<CDisablePadRumble, std::detail::default_delete<CDisablePadRumble>>`.
- Control disposal at `0x8012FC70` loads the owned pointer at twelve and tail-calls `0x801B4BF0` with deletion flag one.

Both class bodies contain no instance-field accesses or virtual-pointer stores. The allocation of one byte and the direct destructor call establish a complete empty nonvirtual class shape, with a constructor and destructor. The original class spelling is high confidence; it is not an invented opaque storage type. No class header or source selection is published by this finding.

## Behavior and ownership

Construction increments the block count at offset four of each of the four Wii rumble objects and each of the four GameCube rumble objects. It invokes the capability predicate and stops a motor when it cannot run. Destruction reverses those counts and performs the same stop/state handling where required. This is an ordinary scoped inhibition of controller rumble. The destructor's final conditional scalar delete is the standard CodeWarrior deleting-destructor ABI, not an instance member.

The 88-byte rumble object layout, virtual order, cooldown, alarm and start-time fields are established separately in F-315-4. The stop transitions cancel the alarm, stop the motor, and schedule the remaining cooldown when needed. Native operations use the actual OS timer clock and ordinary floating-point conversion; no fixed tick constant is inferred from unrelated SDK units.

The class bodies remain in the original pad fallback. The pad file begins at the #315 callback `0x801B15A0`; these two later bodies are surveyed in #316, and the file extends into #317. No additional code bytes are credited for their identification.

## Paired-single analysis limitation

The isolated IDA reading aid initially omitted function starts at `0x801B4640`, `0x801B4840`, and `0x801B4BF0`. Their native entry prologues and exception-index entries independently establish starts and extents. Defining those bounded functions in the outside database preserves their reading aid, but the generic PowerPC decoder still mistakes paired-single save/load instructions for unrelated opcodes and stops decompilation.

The actual Gekko-aware binutils disassembler correctly decodes the paired-single register saves and loads. Its reading aid was generated from a previously verified source-linked ELF whose pad bytes still come from the original object:

`tools/local/binutils/powerpc-eabi-objdump.exe -d -M gekko --start-address=0x801B4640 --stop-address=0x801B4FC0 <verified-main.elf>`

The disassembly, original entry bytes and independent call/RTTI proofs are retained outside the repository in `C:/MassStorage/GitHub/315-pad-paired-single.txt`, `315-pad-recovered-native.json`, `315-disable-rumble-xrefs.json`, and `315-disable-rumble-callers.json`. The tracked IDA database and frozen executable are unchanged. Raw disassembly and decompiler output are reading aids, not reconstructed source.

## Traps

- A generic PowerPC decoder can reject valid Gekko paired-single prologues and omit real functions. Confirm native entry bytes, exception indices and callers, then use the actual Gekko-aware disassembler; do not infer an absent original function from an absent IDA entry.
- Template control RTTI and the exact allocation size can recover an empty concrete class even when that class has no own polymorphic RTTI. Pair constructor and destructor consumers with the typed control before assigning the identity.
