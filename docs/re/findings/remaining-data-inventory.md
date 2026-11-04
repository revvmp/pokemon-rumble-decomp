# Remaining data inventory after typed-global integration

The accepted maintainer checkpoint has 1,656,655 of 1,788,769 source-linked data bytes, or 92.61425 percent. The inventory below comes from that build report, with per-unit detail retained in ignored tools/local/session/data-inventory.json.

| Remaining category | Bytes | Meaning |
| --- | --- | --- |
| Data attached to source units not yet linked | 111262 | Mostly metadata, literals, and tables belonging to game units still awaiting function or emission completion |
| Embedded system-message U8 asset archive | 11488 | gSystemMessageArchive, asset data; can be handled through a proven asset reconstruction workflow |
| DSP instruction image | 8192 | axDspSlave; executable DSP code, deliberately excluded from raw-byte source copying |
| Source-evidenced alignment gaps | 112 | Two 28-byte gaps before 32-byte-aligned resource names, 24 bytes before the aligned NAND home-directory buffer, and 32 bytes before the 64-byte-aligned NAND logging buffer |
| Other anonymous zero storage or alignment | 1048 | Automatic pad/gap labels alone do not prove original source intent; includes the 808-byte region below |
| Explicit placeholder auto unit | 12 | Includes alignment around the one-byte callback placeholder, described below |

The largest named source-unit data blocks are CDlgPiiSelector (20260), CModel (14460), CLayout (12948), CDlgCollection (11952), CRemoteMemory (10884), CGameResult (9652), CPii (8156), and CPiiListFilterSort (6752). Linking their real source is the main remaining route to data coverage; replacing their metadata independently would obscure that dependency.

The symbol inferred as lbl_804B9028 is the one-byte std::tr1::placeholders::_1 object used by CPiiListFilterSort's preview callback bind expressions. Four native lbz references copy it into the bind argument, including 0x800E83D0. The source includes the static ph<1> declaration from include/tuple. Its owner should gain the small-data split when that unit is source-linked; this identity was sent to the owning agent.

ArchiveFontBase::LOAD_GLYPH_ALL has now moved into the Matching ut_ArchiveFontData.cpp data-only owner. Its one-byte empty-string definition is unchanged; the existing base source compiles with zero function differences after the move. The maintainer reports native ninja exit 0 and frozen SHA PASS. The .sbss2 split is 0x804BBC08?0x804BBC0C, including three alignment bytes. The accepted checkpoint has 874 Matching units.

The largest anonymous region is .bss 0x80453018?0x80453340, 808 bytes, between OSThread's IdleContext and OSStateTM buffers. There is no inferred object or current symbolic reference inside it. This is not enough evidence to claim an invented byte array as source reconstruction. Other anonymous regions are mostly short alignment-sized gaps; any removal from the denominator or source ownership correction requires native layout evidence and transparent accounting.

Source-evidenced alignment ranges are .data 0x803C5744?0x803C5760 and 0x803C5924?0x803C5940 (NW4R_G3D_RESFILE_NAME_DEF uses ATTRIBUTE_ALIGN(32)), .bss 0x8046ECC8?0x8046ECE0 (s_homeDir is aligned(32)), and .bss 0x8046ED20?0x8046ED40 (NANDLogging s_message is aligned(64)). These gaps remain included in the inventory totals; no denominator change was made for this classification.
