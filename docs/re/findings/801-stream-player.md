# F-801-1: Stream player reconstruction and runtime cast prerequisite

Issue #801 contains 45 retained functions and 10732 native instruction bytes in
`nw4r/snd/snd_StrmPlayer.cpp`, from `0x80289F90` through `0x8028CA98`.
The final four functions are compiler-generated `@180@` adjustment thunks,
subtracting 180 from `this` before branching to the ordinary destructor or
SoundThread callback. This proves the callback base at offset `0xB4`.

The reference is the CC0 Skyward Sword reconstruction at
`787b0b1e9d30e3c1d91ced61350b059ffc5a2411`,
`src/nw4r/snd/snd_StrmPlayer.cpp`, with its corresponding complete headers.
The related kiwi515/ogws source was inspected as a second reference, but its
earlier player layout differs from this target.

## Runtime cast prerequisite

The pinned `ut_RuntimeTypeInfo.h` tests the input pointer before dereferencing
its vtable. Main's existing template omitted that check. Native
`LoadStreamData` at `0x8028AFB0` explicitly tests `r4` for zero before its
virtual runtime-type-info call. The ordinary nested pointer guard restores
that behavior. With the guard and the coordinated complete Voice interface,
the scratch source reproduces all non-relocation instruction words and exact
sizes of the 41 named retained functions. The three consumers previously
differing are `LoadStreamData` (624 bytes), `Update` (440 bytes), and
`UpdateBuffer` (468 bytes).

This is an instruction comparison, not a matching claim. Paired relocations,
initialized data, source selection, and the frozen full executable still
require their own gates.

## Remaining source evidence

The unmodified reference contains a fake `float_order_snd_StrmPlayer` function
and local `ppc_iro_level` overrides. Neither belongs in reconstructed source.
The ordinary candidate needs neither override to reproduce retained
non-relocation instructions after restoring the runtime cast guard.

Its constant pool order still differs from native data. The ordinary candidate
emits 1.0, 0.0, 127.0, 63.0, unsigned conversion, signed conversion. Native
references instead identify 0.0 at `0x804BAFF0`, signed conversion at
`0x804BAFF8`, unsigned conversion at `0x804BB000`, 1.0 at `0x804BB008`,
127.0 at `0x804BB00C`, and 63.0 at `0x804BB010`. No forced constants,
dummy function, raw data copy, or speculative compiler option has been added.
The source is not selected as Matching while this remains unresolved.

The independent Wii symbol maps in
[`riptl/wii-symbols` at `34d03db117a7e0007ca3ef3c8bc7402dad1c2925`](https://github.com/riptl/wii-symbols/blob/34d03db117a7e0007ca3ef3c8bc7402dad1c2925/symbols/SSQE01.txt)
identify a real 432-byte `GetFilledBufferPercentage` method immediately before
the 308-byte `InitParam`, also present in the SC8P01 map. This gives a concrete
missing-method lead rather than evidence for the reference's dummy function.
No Wii implementation was found in the inspected CC0 references. A later
NintendoWare4Ctr implementation is GPL-3.0 and was not imported or adapted.

## Traps

- A reference optimization override concealed a missing null check in the
  shared runtime cast template. Inspect the common source used by differing
  consumers before changing compiler settings. An early return before the
  type-info declaration reproduces semantics but changes scheduling; the
  pinned nested guard after type-info acquisition reproduces these native
  consumers.
- Non-relocation instruction identity can hide constant-pool differences.
  Reconstruct and pair every instruction relocation and verify initialized
  storage before claiming a matching unit.

## Shared-header validation

Before editing the runtime cast header, a successful default build recorded
468 existing source objects' allocated section contents and normalized
relocations. After adding only the nested pointer guard, `python configure.py`
and `.venv/Scripts/ninja` both exited 0. All 468 recorded objects retained
exact code, initialized data, zero-storage extents, and normalized relocation
targets. The actual rebuilt `build/wpse-v1/main.dol` passed the frozen manifest
verifier and retained SHA-256
`ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.
No source unit, split, symbol extent, progress count, or compiler setting is
changed by this prerequisite.

The final integration gate rebases this header-only change onto main
`ec2e1acd`, after #835 and #843. Configure and plain Ninja against the
previously clean `build/801-header1` output exit zero; the actual-directory
frozen verifier passes again, and all 468 prior source objects retain exact
code, data and relocation targets. No metadata entry or shared-header conflict
required resolution. The independently coordinated #809 Layout consumer also
reproduces all 19 retained functions, 4144 instruction bytes and 98 instruction
relocation targets with this exact header, without publishing a duplicate fix.
