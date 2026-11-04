# Embedded system message resources

Seven BMG message resources occupy0x803620C0 through0x80363A80, totaling6592bytes. Their existing globals name Japanese, English, European English, German, French, Spanish and Italian variants. Every resource has the MESGbmg1 file signature, UTF-16 encoding, two blocks, seven eight-byte message entries, a96-byte INF1 block and a DAT1 text block. The original24-byte INF1 padding and trailing file padding are zero.

SystemMessageResource is a typed serialization view built from the existing CMsgRes header/message structures, fixed metadata and a sized wchar_t text array. Each definition contains the original readable message text as ordinary wide literals, with Unicode escapes for non-ASCII characters and embedded terminators. The source contains no executable bytes, pointer fixups or runtime extraction step. Main now declares the actual resource types and passes their addresses, preserving all compiled code and relocations. These are reconstructed resource data, not additional decompiled function credit.

The original objects are writable resources despite occupying .rodata: CMsgRes performs escape conversion in place. The new definitions preserve non-const types while selecting their original linker section. Ordinary compiler initialization supplies the documented format padding. All6592serializedbytes match through a fresh successful native build and frozen main.dol verifierPASS. No original hash changed.

The adjacent fatal_errmsg U8 archive remains an explicit original-data fallback. It contains three BRLAN animations, a BRLYT layout and two TPL textures; it is not conflated with these typed message definitions or counted as reconstructed source here.
