# F-218-2: shared map and move-description table

The native table constructor at `0x8010DDE0` and cell accessor at `0x80110100` are shared by the map-description and move-description paths. The previous WazaDescData storage-only declaration and the newer MapPiiTable declaration described the same 44-byte contract. WazaDescData is retained as a descriptive alias of the complete MapPiiTable. CStatistic remains a separate class; the session owns it independently.

## Layout and API evidence

The constructor writes a twelve-byte vector of eight-byte NPpu::CValue records at zero, twelve-byte string and wide-string intern sets at twelve and twenty-four, and integer row/column counts at thirty-six and forty. The MapDesc table remains at offset twelve; MapDesc remains 56 bytes. Session move-description storage remains 44 bytes. The existing nonconst `NPpu::CValue* GetCell(int, int)` and inline row-count accessor are preserved.

Native `GetCell` indexes `(row + 1) * columnCount + column`. The same native function is called by CWaza through the session move-description accessor and by CMsgBalloon through MapDesc. An independent typed MapPiiTable constructor produces all sixty bytes at `0x8010DDE0`; its GetCell producer produces all thirty-two bytes at `0x80110100`. Constructor and destructor symbol names were previously inferred from the move-description use and are corrected to the shared canonical table type; their sizes and splits are unchanged. No outside function body is selected from source by this prerequisite.

## Consumer controls

The actual CGameSession producer retains all 197 function extents and non-relocation instruction streams after the alias/type completion. Its table constructor and destructor relocations now name MapPiiTable. CMsgBalloon retains all 511 function extents and non-relocation streams. CWaza retains its complete fourteen functions / 4,128 instruction bytes, with every direct branch target paired, including canonical MapPiiTable::GetCell at `0x80110100`.

Raw independent controls are preserved outside the checkout in `C:/MassStorage/GitHub/218-table-alias-control`: table-producer.cpp/.o, both actual baseline/candidate consumer objects, table-consumer-controls.json, CWaza.cpp/.o, and 218-code-target-audit.json. The game compiler and flags are unchanged.

The bounded header prerequisite also passed `python configure.py` (0), `.venv/Scripts/ninja` (0), and the explicit frozen-manifest `verify.py verify` command (0), with a freshly produced `main.dol`. Existing selected source consumers remain byte-identical.

## Traps

A context-specific storage name is not proof of a distinct native type. Both callers reach the same table implementation; preserve the canonical public API and establish the shared layout rather than emitting a second differently named accessor at the same address. Earlier findings inferred CStatistic identity without primary name evidence; the session's separate CStatistic ownership disproves adopting that conflation here.
