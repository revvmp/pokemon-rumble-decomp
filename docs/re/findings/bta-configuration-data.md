# BTE configuration data reconstruction

Three data-only configuration units reconstruct 197 bytes of initialized data and zero storage from existing typed BTE structures. No executable bytes, raw object blobs, inline assembly, or DSP microcode are used. Existing bta_dm_act, bta_dm_pm, and bta_sys_main consumers prove the field meanings and pointer owners. The pinned Petari configuration sources independently supply original names and table structure: [device manager](https://raw.githubusercontent.com/SMGCommunity/Petari/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/bte/bta_dm_cfg.c), [HID host](https://raw.githubusercontent.com/SMGCommunity/Petari/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/bte/bta_hh_cfg.c), and [system](https://raw.githubusercontent.com/SMGCommunity/Petari/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/bte/bta_sys_cfg.c). [License notices](../licenses/bta-configuration-NOTICE.md) preserve their Apache provenance.

| Unit | Section | Exact exclusive range | Bytes |
| --- | --- | --- | --- |
| bta_dm_cfg.c | .rodata | 0x80397E48?0x80397ED4 | 140 |
| bta_dm_cfg.c | .sdata | 0x804B8EB0?0x804B8EC4 | 20 |
| bta_dm_cfg.c | .sbss2 | 0x804BBC18?0x804BBC1C | 4 (three object bytes, one alignment byte) |
| bta_hh_cfg.c | .rodata | 0x80397ED8?0x80397EE4 | 12 |
| bta_hh_cfg.c | .sdata | 0x804B8EC8?0x804B8ED4 | 12 |
| bta_sys_cfg.c | .sdata | 0x804B8ED8?0x804B8EDC | 4 |
| bta_sys_cfg.c | .sdata2 | 0x804BB5F0?0x804BB5F6 | 6 |

The device manager config carries class-of-device bytes, link policy and timeouts; role and compression tables; nine pairs of power-state actions; and sniff/park power parameters. The zero three-byte power-policy entry has zero enabled services. HID host data holds four device-type/app-ID pairs and a 1024-byte SDP database limit. System config selects mailbox event 4, mailbox 2, timer 1, and trace level 0.

Isolated GC/3.0a3 compilation with the configured BTE SDK flags succeeds for all three units. Each complete emitted data section compares byte-for-byte with its original range after resolving compiler-generated address relocations to the verified original symbols; the zero section contains exactly three zero bytes. This relocation resolution is diagnostic only; no transformed object enters the native build. Evidence and integration mapping are under ignored tools/local/session/bta-config, with reproducer bta-config-verify.py. Native source-link acceptance remains pending maintainer integration.

## Traps

The inferred old symbol extents include alignment padding: bta_dm_cfg is 10 bytes rather than 12, power spec 74 rather than 76, the two power-mode structures total 20 rather than 24, HID config is 12 rather than 16, and system config is 6 rather than 8. End splits at actual structure ends and leave following padding to the next aligned object. The four zeros at 0x80397E44 are preceding padding, not a configuration field. The public reference's p_bta_dm_cfg pointer is absent here: target consumers directly address bta_dm_cfg, so adding that pointer would introduce unowned data.

## Native acceptance

The maintainer integrated all three units as Matching. Ninja exited 0 and the frozen-manifest verifier passed. The .sbss2 split must end at 0x804BBC1C, rounding the three-byte object to four bytes: dtk rejects an automatic continuation beginning at 0x804BBC1B because it violates section alignment. The compiler still emits a three-byte object; native alignment supplies the fourth zero byte. After integrating actual object extents, the data denominator is 1,788,801 bytes and source-linked data is 1,656,447 bytes (92.60097 percent), with 868 Matching units. These coverage totals are the maintainer's accepted checkpoint, not a claim that all remaining data is reconstructed.
