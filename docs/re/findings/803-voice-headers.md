# F-803-1: Voice source declarations and flat channel ADPCM fields

Target `wpse-v1`. This bounded addition to the F-804-1 sound-header prerequisite supplies original Voice constant/helper declarations and corrects ChannelParam's field grouping. It changes no layout, base class, virtual method or source selection beyond the separately established F-804-1 target Voice layout.

## Target and reference evidence

CC0 zeldaret/ss `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, `snd_Voice.cpp` and its header, supplies the nine float constant names, voice priority/filter bounds and CalcAxBiquadFilter declaration. These are nonvirtual declarations; no initialized data is added by this header change. The complete Voice source probe reproduces all 46 original #803 native function sizes and all non-relocated instruction words with otherwise unchanged Wii/1.1 NW4R flags. It remains diagnostic until the later normal source-selected relocation/data/full-artifact gate.

ChannelParam remains 0x34 bytes, aligned to four: dataAddr at 0, AdpcmParam at 4, and AdpcmLoopParam at 0x2C, with natural trailing padding. WaveInfo remains 0x80 bytes with its two-channel array at 0x18. The original Voice::Setup at `0x8028E6C0` copies the ADPCM state from WaveInfo +0x1C, takes its loop state at +0x44 and advances both channel cursors by 0x34. The reference models these as direct adpcmParam and adpcmLoopParam fields. The imported nested AdpcmInfo field preserved bytes but changed pointer induction and register allocation, adding eight bytes. The direct reference fields restore all 0x200 native Setup bytes without casts, padding members or layout changes.

A repository search finds no current main source access to ChannelParam.adpcmInfo. The only pending accesses are two lines in the owned #443 WaveFile source in combined PR #753; that source PR updates those two field accesses, then repeats its actual repository-only compilation and native/full-link checks with these canonical headers. AdpcmInfo itself remains available and unchanged for the binary file structure and other APIs.

## Unchanged-source proof

The fresh unchanged main baseline is `4da1cdac38490f2d1738b36b5667b00ca94f8eaa`. Configure, full default Ninja, all_source and explicit frozen verification pass before saving all 468 actual registered native source outputs and the four affected canonical headers. Applying only the combined four-header prerequisite, with no source changes or scratch include paths, passes the same real commands. Of 468 object files, 466 are completely byte-identical. AxManager and Channel differ only in numeric compiler-local names in .strtab; every allocated section, raw RELA entry, symbol value/extent/type/order is identical.

The rebuilt DOL directly equals all 4,094,880 frozen reference bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This source-unchanged equivalence is scoped to its recorded baseline. The exact final header PR head separately repeats configure, default Ninja, all_source and actual-output frozen verification after its final latest-main rebase. No reconstructed byte credit or issue closure is claimed by these declarations. Scope excludes the boot loader, other package contents and WAD packaging.

The actual repository-only combined source consumer checkpoint is `3924731f` in PR #753, including the four canonical headers and the two owned WaveFile field-use updates. Configure, all_source, normal default Ninja and explicit frozen verification exit 0, without scratch include paths. All seven units, their 52 retained functions and all allocated sections score 100% in objdiff. The independent native audits still pass for all 6,900 bytes, 233 code and 31 vtable/constructor relocations, retained data and final addresses. The separate original #444 inventory remains 37/37 functions / 5,236 native bytes. The two field-use edits stay in the source PR; this header prerequisite contains no source changes.

## Traps

- Nested and direct aggregate fields can have identical ABI yet compile differently. Here the source's aggregate assignment generated an additional induction pointer only through the nested field. The pinned direct-field declaration, typed original offsets and full native Setup body establish the correction; no arbitrary byte view or synthetic padding is needed.
- The imported LPF helper returns int. Retaining the cutoff as an int and narrowing at SetLpf matches the original placement of its integer mask; an early u16 local moves two instructions.
