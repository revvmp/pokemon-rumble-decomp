# Workstreams

Every workstream is a GitHub issue. Claim work with `tools/collab/claim_issue.py` or by telling Claude Code to take an issue (the `take-issue` skill), as described in [collaboration](COLLABORATION.md). Maintainers keep this file current; contributors do not edit it.

Decomp issues are blocks of about 25 functions in address order, 507 in total. Every function in `config/wpse-v1/symbols.txt` is listed in exactly one issue body, except the already-matched `__init_cpp_exceptions` functions. Regions come from the executable survey in [F-0012](re/FINDINGS.md#f-0012-full-executable-layout-survey) and compilers from [F-0007](re/FINDINGS.md#f-0007-per-library-compiler-identification). Block boundaries are estimates; establishing exact unit boundaries is part of each issue.

## Research and infrastructure

| Issue | Title |
| --- | --- |
| #25 | NW4R: resolve Wii/1.0 versus Wii/1.1 |
| #29 | Game: recover names from debug and script strings |
| #35 | Toolchain: confirm the original linker version |
| #36 | Boot loader: decide reconstruction scope for content 6 |

## Game

286 issues, 7,264 functions.

| Issue | Title | Functions |
| --- | --- | --- |
| #38 | Game: BController, BModelObj, BWorld (1/15): 0x80052500–0x800528d8 | 19 |
| #39 | Game: BController, BModelObj, BWorld (2/15): 0x800528e0–0x800561a4 | 29 |
| #40 | Game: BController, BModelObj, BWorld (3/15): 0x800561b0–0x80058b44 | 26 |
| #41 | Game: BController, BModelObj, BWorld (4/15): 0x80058b50–0x8005a8c4 | 25 |
| #42 | Game: BController, BModelObj, BWorld (5/15): 0x8005a8d0–0x8005ac6c | 24 |
| #43 | Game: BController, BModelObj, BWorld (6/15): 0x8005ac70–0x8005b814 | 29 |
| #44 | Game: BController, BModelObj, BWorld (7/15): 0x8005b820–0x8005d364 | 25 |
| #45 | Game: BController, BModelObj, BWorld (8/15): 0x8005d370–0x8005f444 | 26 |
| #46 | Game: BController, BModelObj, BWorld (9/15): 0x8005f450–0x80062974 | 27 |
| #47 | Game: BController, BModelObj, BWorld (10/15): 0x80062980–0x80064ba4 | 24 |
| #48 | Game: BController, BModelObj, BWorld (11/15): 0x80064bb0–0x80065864 | 25 |
| #49 | Game: BController, BModelObj, BWorld (12/15): 0x80065870–0x80066454 | 24 |
| #50 | Game: BController, BModelObj, BWorld (13/15): 0x80066460–0x80068610 | 38 |
| #51 | Game: BController, BModelObj, BWorld (14/15): 0x80068610–0x8006a100 | 27 |
| #52 | Game: BController, BModelObj, BWorld (15/15): 0x8006a100–0x8006b464 | 31 |
| #53 | Game: HUD helpers, controllers, CDisconMsg (1/7): 0x8006b470–0x8006bee0 | 20 |
| #54 | Game: HUD helpers, controllers, CDisconMsg (2/7): 0x8006bee0–0x8006c868 | 22 |
| #55 | Game: HUD helpers, controllers, CDisconMsg (3/7): 0x8006c870–0x8006ddd8 | 25 |
| #56 | Game: HUD helpers, controllers, CDisconMsg (4/7): 0x8006dde0–0x8006e6a0 | 18 |
| #57 | Game: HUD helpers, controllers, CDisconMsg (5/7): 0x8006e6a0–0x8006fbe4 | 26 |
| #58 | Game: HUD helpers, controllers, CDisconMsg (6/7): 0x8006fbf0–0x800709d4 | 25 |
| #59 | Game: HUD helpers, controllers, CDisconMsg (7/7): 0x800709e0–0x80071218 | 16 |
| #60 | Game: CDividedGround (1/6): 0x80071220–0x80072274 | 15 |
| #61 | Game: CDividedGround (2/6): 0x80072280–0x800740f4 | 28 |
| #62 | Game: CDividedGround (3/6): 0x80074100–0x80075524 | 20 |
| #63 | Game: CDividedGround (4/6): 0x80075530–0x80075ac4 | 14 |
| #64 | Game: CDividedGround (5/6): 0x80075ad0–0x800761e4 | 24 |
| #65 | Game: CDividedGround (6/6): 0x800761f0–0x800776a8 | 35 |
| #66 | Game: CDlgCollection, CDlgInfo (1/7): 0x800776b0–0x8007afd4 | 24 |
| #67 | Game: CDlgCollection, CDlgInfo (2/7): 0x8007afe0–0x8007c674 | 25 |
| #68 | Game: CDlgCollection, CDlgInfo (3/7): 0x8007c680–0x8007d484 | 26 |
| #69 | Game: CDlgCollection, CDlgInfo (4/7): 0x8007d490–0x8007e5c4 | 38 |
| #70 | Game: CDlgCollection, CDlgInfo (5/7): 0x8007e5d0–0x80082694 | 28 |
| #71 | Game: CDlgCollection, CDlgInfo (6/7): 0x800826a0–0x800841f4 | 28 |
| #72 | Game: CDlgCollection, CDlgInfo (7/7): 0x80084200–0x80084ddc | 20 |
| #73 | Game: CDlgMsgWin, CDlgPiiList (1/19): 0x80084de0–0x800852f4 | 22 |
| #74 | Game: CDlgMsgWin, CDlgPiiList (2/19): 0x80085300–0x80085a54 | 25 |
| #75 | Game: CDlgMsgWin, CDlgPiiList (3/19): 0x80085a60–0x80086394 | 25 |
| #76 | Game: CDlgMsgWin, CDlgPiiList (4/19): 0x800863a0–0x80086e24 | 24 |
| #77 | Game: CDlgMsgWin, CDlgPiiList (5/19): 0x80086e30–0x80087514 | 25 |
| #78 | Game: CDlgMsgWin, CDlgPiiList (6/19): 0x80087520–0x80087a74 | 26 |
| #79 | Game: CDlgMsgWin, CDlgPiiList (7/19): 0x80087a80–0x80087f54 | 26 |
| #80 | Game: CDlgMsgWin, CDlgPiiList (8/19): 0x80087f60–0x80088964 | 31 |
| #81 | Game: CDlgMsgWin, CDlgPiiList (9/19): 0x80088970–0x800896d8 | 22 |
| #82 | Game: CDlgMsgWin, CDlgPiiList (10/19): 0x800896e0–0x8008acb4 | 12 |
| #83 | Game: CDlgMsgWin, CDlgPiiList (11/19): 0x8008acc0–0x8008d434 | 22 |
| #84 | Game: CDlgMsgWin, CDlgPiiList (12/19): 0x8008d440–0x8008ee64 | 25 |
| #85 | Game: CDlgMsgWin, CDlgPiiList (13/19): 0x8008ee70–0x8008fd24 | 26 |
| #86 | Game: CDlgMsgWin, CDlgPiiList (14/19): 0x8008fd30–0x800906d4 | 24 |
| #87 | Game: CDlgMsgWin, CDlgPiiList (15/19): 0x800906e0–0x80091604 | 25 |
| #88 | Game: CDlgMsgWin, CDlgPiiList (16/19): 0x80091610–0x80091d54 | 24 |
| #89 | Game: CDlgMsgWin, CDlgPiiList (17/19): 0x80091d60–0x80092254 | 25 |
| #90 | Game: CDlgMsgWin, CDlgPiiList (18/19): 0x80092260–0x80092624 | 25 |
| #91 | Game: CDlgMsgWin, CDlgPiiList (19/19): 0x80092630–0x80092f6c | 34 |
| #92 | Game: CDlgPiiSelector (1/21): 0x80092f70–0x80093534 | 23 |
| #93 | Game: CDlgPiiSelector (2/21): 0x80093540–0x80093f64 | 27 |
| #94 | Game: CDlgPiiSelector (3/21): 0x80093f70–0x80095154 | 24 |
| #95 | Game: CDlgPiiSelector (4/21): 0x80095160–0x800962e4 | 27 |
| #96 | Game: CDlgPiiSelector (5/21): 0x800962f0–0x80096954 | 25 |
| #97 | Game: CDlgPiiSelector (6/21): 0x80096960–0x80097794 | 23 |
| #98 | Game: CDlgPiiSelector (7/21): 0x800977a0–0x800986f4 | 32 |
| #99 | Game: CDlgPiiSelector (8/21): 0x80098700–0x800990b4 | 26 |
| #100 | Game: CDlgPiiSelector (9/21): 0x800990c0–0x8009abf4 | 27 |
| #101 | Game: CDlgPiiSelector (10/21): 0x8009ac00–0x8009b614 | 24 |
| #102 | Game: CDlgPiiSelector (11/21): 0x8009b620–0x8009bfc4 | 24 |
| #103 | Game: CDlgPiiSelector (12/21): 0x8009bfd0–0x8009c7d4 | 23 |
| #104 | Game: CDlgPiiSelector (13/21): 0x8009c7e0–0x8009cff4 | 25 |
| #105 | Game: CDlgPiiSelector (14/21): 0x8009d000–0x8009d5a4 | 25 |
| #106 | Game: CDlgPiiSelector (15/21): 0x8009d5b0–0x8009db04 | 25 |
| #107 | Game: CDlgPiiSelector (16/21): 0x8009db10–0x8009dfe4 | 25 |
| #108 | Game: CDlgPiiSelector (17/21): 0x8009dff0–0x8009e664 | 16 |
| #109 | Game: CDlgPiiSelector (18/21): 0x8009e670–0x8009ef24 | 25 |
| #110 | Game: CDlgPiiSelector (19/21): 0x8009ef30–0x8009f2a4 | 24 |
| #111 | Game: CDlgPiiSelector (20/21): 0x8009f2b0–0x8009f614 | 21 |
| #112 | Game: CDlgPiiSelector (21/21): 0x8009f620–0x8009fd38 | 38 |
| #113 | Game: CDropPii, CEffect, CEnemyInfo, CEnemyPii, CFacility (1/3): 0x8009fd40–0x800a1d34 | 25 |
| #114 | Game: CDropPii, CEffect, CEnemyInfo, CEnemyPii, CFacility (2/3): 0x800a1d40–0x800a3b08 | 37 |
| #115 | Game: CDropPii, CEffect, CEnemyInfo, CEnemyPii, CFacility (3/3): 0x800a3b10–0x800a4ee8 | 26 |
| #116 | Game: CGameEnding, CGameMap, CGameResult (1/10): 0x800a4ef0–0x800a5be4 | 15 |
| #117 | Game: CGameEnding, CGameMap, CGameResult (2/10): 0x800a5bf0–0x800a6444 | 21 |
| #118 | Game: CGameEnding, CGameMap, CGameResult (3/10): 0x800a6450–0x800a7650 | 30 |
| #119 | Game: CGameEnding, CGameMap, CGameResult (4/10): 0x800a7650–0x800a98d4 | 23 |
| #120 | Game: CGameEnding, CGameMap, CGameResult (5/10): 0x800a98e0–0x800aa4e8 | 25 |
| #121 | Game: CGameEnding, CGameMap, CGameResult (6/10): 0x800aa4f0–0x800aaa34 | 24 |
| #122 | Game: CGameEnding, CGameMap, CGameResult (7/10): 0x800aaa40–0x800ac4a8 | 40 |
| #123 | Game: CGameEnding, CGameMap, CGameResult (8/10): 0x800ac4b0–0x800af494 | 26 |
| #124 | Game: CGameEnding, CGameMap, CGameResult (9/10): 0x800af4a0–0x800b1574 | 25 |
| #125 | Game: CGameEnding, CGameMap, CGameResult (10/10): 0x800b1580–0x800b2034 | 19 |
| #126 | Game: CGameSession (1/10): 0x800b2040–0x800b32d4 | 25 |
| #127 | Game: CGameSession (2/10): 0x800b32e0–0x800b3fe4 | 23 |
| #128 | Game: CGameSession (3/10): 0x800b3ff0–0x800b5664 | 25 |
| #129 | Game: CGameSession (4/10): 0x800b5670–0x800b6c84 | 23 |
| #130 | Game: CGameSession (5/10): 0x800b6c90–0x800b8144 | 17 |
| #131 | Game: CGameSession (6/10): 0x800b8150–0x800b9334 | 24 |
| #133 | Game: CGameSession (7/10): 0x800b9340–0x800bab84 | 17 |
| #134 | Game: CGameSession (8/10): 0x800bab90–0x800bb584 | 25 |
| #135 | Game: CGameSession (9/10): 0x800bb590–0x800bc2f4 | 25 |
| #136 | Game: CGameSession (10/10): 0x800bc300–0x800bcb34 | 18 |
| #137 | Game: CGameStage, HUD and layout manager (1/5): 0x800bcb40–0x800be9d8 | 40 |
| #138 | Game: CGameStage, HUD and layout manager (2/5): 0x800be9e0–0x800c0c58 | 31 |
| #139 | Game: CGameStage, HUD and layout manager (3/5): 0x800c0c60–0x800c26cc | 25 |
| #140 | Game: CGameStage, HUD and layout manager (4/5): 0x800c26d0–0x800c495c | 22 |
| #141 | Game: CGameStage, HUD and layout manager (5/5): 0x800c4960–0x800c6090 | 23 |
| #142 | Game: CMsgBalloon, CMultiRanking (1/18): 0x800c6090–0x800c6684 | 24 |
| #143 | Game: CMsgBalloon, CMultiRanking (2/18): 0x800c6690–0x800c6be4 | 23 |
| #144 | Game: CMsgBalloon, CMultiRanking (3/18): 0x800c6bf0–0x800c70f4 | 25 |
| #145 | Game: CMsgBalloon, CMultiRanking (4/18): 0x800c7100–0x800c7ee4 | 22 |
| #146 | Game: CMsgBalloon, CMultiRanking (5/18): 0x800c7ef0–0x800c88c8 | 32 |
| #147 | Game: CMsgBalloon, CMultiRanking (6/18): 0x800c88d0–0x800c9424 | 24 |
| #148 | Game: CMsgBalloon, CMultiRanking (7/18): 0x800c9430–0x800c9764 | 25 |
| #149 | Game: CMsgBalloon, CMultiRanking (8/18): 0x800c9770–0x800ca004 | 26 |
| #150 | Game: CMsgBalloon, CMultiRanking (9/18): 0x800ca010–0x800ca8e4 | 25 |
| #151 | Game: CMsgBalloon, CMultiRanking (10/18): 0x800ca8f0–0x800caef4 | 24 |
| #152 | Game: CMsgBalloon, CMultiRanking (11/18): 0x800caf00–0x800cb334 | 24 |
| #153 | Game: CMsgBalloon, CMultiRanking (12/18): 0x800cb340–0x800cba84 | 24 |
| #154 | Game: CMsgBalloon, CMultiRanking (13/18): 0x800cba90–0x800cbcf4 | 24 |
| #155 | Game: CMsgBalloon, CMultiRanking (14/18): 0x800cbd00–0x800cc9f4 | 25 |
| #156 | Game: CMsgBalloon, CMultiRanking (15/18): 0x800cca00–0x800cce74 | 25 |
| #157 | Game: CMsgBalloon, CMultiRanking (16/18): 0x800cce80–0x800cd174 | 25 |
| #158 | Game: CMsgBalloon, CMultiRanking (17/18): 0x800cd180–0x800cd7f8 | 30 |
| #159 | Game: CMsgBalloon, CMultiRanking (18/18): 0x800cd800–0x800d00b0 | 25 |
| #160 | Game: CNoticeDisplay, CNullObj, CNumericInput (1/7): 0x800d00b0–0x800d1024 | 20 |
| #161 | Game: CNoticeDisplay, CNullObj, CNumericInput (2/7): 0x800d1030–0x800d2bd8 | 12 |
| #162 | Game: CNoticeDisplay, CNullObj, CNumericInput (3/7): 0x800d2be0–0x800d39d4 | 21 |
| #163 | Game: CNoticeDisplay, CNullObj, CNumericInput (4/7): 0x800d39e0–0x800d44b4 | 24 |
| #164 | Game: CNoticeDisplay, CNullObj, CNumericInput (5/7): 0x800d44c0–0x800d4b50 | 22 |
| #165 | Game: CNoticeDisplay, CNullObj, CNumericInput (6/7): 0x800d4b50–0x800d6790 | 28 |
| #166 | Game: CNoticeDisplay, CNullObj, CNumericInput (7/7): 0x800d6790–0x800d83b0 | 20 |
| #167 | Game: CPauseMenu, CPii (1/6): 0x800d83b0–0x800dca04 | 37 |
| #168 | Game: CPauseMenu, CPii (2/6): 0x800dca10–0x800df194 | 28 |
| #169 | Game: CPauseMenu, CPii (3/6): 0x800df1a0–0x800e0064 | 20 |
| #170 | Game: CPauseMenu, CPii (4/6): 0x800e0070–0x800e1ad0 | 24 |
| #171 | Game: CPauseMenu, CPii (5/6): 0x800e1ad0–0x800e3904 | 22 |
| #172 | Game: CPauseMenu, CPii (6/6): 0x800e3910–0x800e5618 | 29 |
| #173 | Game: Pii data (1/8): 0x800e5620–0x800e7a44 | 26 |
| #174 | Game: Pii data (2/8): 0x800e7a50–0x800ea634 | 25 |
| #175 | Game: Pii data (3/8): 0x800ea640–0x800ec5c4 | 22 |
| #176 | Game: Pii data (4/8): 0x800ec5d0–0x800ecfb4 | 37 |
| #177 | Game: Pii data (5/8): 0x800ecfc0–0x800f04e4 | 21 |
| #178 | Game: Pii data (6/8): 0x800f04f0–0x800f3844 | 22 |
| #179 | Game: Pii data (7/8): 0x800f3850–0x800f4fe4 | 21 |
| #180 | Game: Pii data (8/8): 0x800f4ff0–0x800f75f0 | 32 |
| #181 | Game: CPlayer, CPlayerPii (1/13): 0x800f75f0–0x800f8044 | 26 |
| #182 | Game: CPlayer, CPlayerPii (2/13): 0x800f8050–0x800f8484 | 20 |
| #183 | Game: CPlayer, CPlayerPii (3/13): 0x800f8490–0x800f8d5c | 37 |
| #184 | Game: CPlayer, CPlayerPii (4/13): 0x800f8d60–0x800f96b4 | 30 |
| #185 | Game: CPlayer, CPlayerPii (5/13): 0x800f96c0–0x800f9b84 | 26 |
| #186 | Game: CPlayer, CPlayerPii (6/13): 0x800f9b90–0x800fa364 | 23 |
| #187 | Game: CPlayer, CPlayerPii (7/13): 0x800fa370–0x800fabb4 | 25 |
| #188 | Game: CPlayer, CPlayerPii (8/13): 0x800fabc0–0x800fb4d4 | 24 |
| #189 | Game: CPlayer, CPlayerPii (9/13): 0x800fb4e0–0x800fbbd4 | 23 |
| #190 | Game: CPlayer, CPlayerPii (10/13): 0x800fbbe0–0x800fc074 | 28 |
| #191 | Game: CPlayer, CPlayerPii (11/13): 0x800fc080–0x800fc4c4 | 26 |
| #192 | Game: CPlayer, CPlayerPii (12/13): 0x800fc4d0–0x800fcd44 | 25 |
| #193 | Game: CPlayer, CPlayerPii (13/13): 0x800fcd50–0x800fd4f8 | 24 |
| #194 | Game: CPostEffect, CPpuEngine, CPpuEnv (1/5): 0x800fd500–0x800fe5f0 | 13 |
| #195 | Game: CPostEffect, CPpuEngine, CPpuEnv (2/5): 0x800fe5f0–0x80100e74 | 23 |
| #196 | Game: CPostEffect, CPpuEngine, CPpuEnv (3/5): 0x80100e80–0x801035c8 | 32 |
| #197 | Game: CPostEffect, CPpuEngine, CPpuEnv (4/5): 0x801035d0–0x80105de4 | 25 |
| #198 | Game: CPostEffect, CPpuEngine, CPpuEnv (5/5): 0x80105df0–0x80108290 | 28 |
| #199 | Game: CSaveManager (1/8): 0x80108290–0x80108b84 | 27 |
| #200 | Game: CSaveManager (2/8): 0x80108b90–0x801090e4 | 26 |
| #201 | Game: CSaveManager (3/8): 0x801090f0–0x80109754 | 27 |
| #202 | Game: CSaveManager (4/8): 0x80109760–0x80109fc4 | 32 |
| #203 | Game: CSaveManager (5/8): 0x80109fd0–0x8010a7d4 | 24 |
| #204 | Game: CSaveManager (6/8): 0x8010a7e0–0x8010b324 | 26 |
| #205 | Game: CSaveManager (7/8): 0x8010b330–0x8010b7a4 | 25 |
| #206 | Game: CSaveManager (8/8): 0x8010b7b0–0x8010bd28 | 30 |
| #207 | Game: cameras, ground, statistics (1/4): 0x8010bd30–0x8010d7d0 | 33 |
| #208 | Game: cameras, ground, statistics (2/4): 0x8010d7d0–0x80110134 | 26 |
| #209 | Game: cameras, ground, statistics (3/4): 0x80110140–0x80112ee8 | 34 |
| #210 | Game: cameras, ground, statistics (4/4): 0x80112ef0–0x80118180 | 25 |
| #211 | Game: title, tutorial, UI widgets (1/10): 0x80118180–0x80119018 | 24 |
| #212 | Game: title, tutorial, UI widgets (2/10): 0x80119020–0x8011ab90 | 17 |
| #213 | Game: title, tutorial, UI widgets (3/10): 0x8011ab90–0x8011b3d4 | 26 |
| #214 | Game: title, tutorial, UI widgets (4/10): 0x8011b3e0–0x8011bec4 | 25 |
| #215 | Game: title, tutorial, UI widgets (5/10): 0x8011bed0–0x8011c7a8 | 31 |
| #216 | Game: title, tutorial, UI widgets (6/10): 0x8011c7b0–0x8011d7e0 | 40 |
| #217 | Game: title, tutorial, UI widgets (7/10): 0x8011d7e0–0x8011fc78 | 29 |
| #218 | Game: title, tutorial, UI widgets (8/10): 0x8011fc80–0x80121338 | 27 |
| #219 | Game: title, tutorial, UI widgets (9/10): 0x80121340–0x80122294 | 24 |
| #220 | Game: title, tutorial, UI widgets (10/10): 0x801222a0–0x801248c0 | 24 |
| #221 | Game: system, file loading, main.cpp (1/10): 0x801248c0–0x80124fe4 | 23 |
| #222 | Game: system, file loading, main.cpp (2/10): 0x80124ff0–0x801263e8 | 32 |
| #223 | Game: system, file loading, main.cpp (3/10): 0x801263f0–0x801288e4 | 14 |
| #224 | Game: system, file loading, main.cpp (4/10): 0x801288f0–0x8012b5e8 | 40 |
| #225 | Game: system, file loading, main.cpp (5/10): 0x8012b5f0–0x8012d8a4 | 28 |
| #226 | Game: system, file loading, main.cpp (6/10): 0x8012d8b0–0x8012e934 | 26 |
| #227 | Game: system, file loading, main.cpp (7/10): 0x8012e940–0x8012f8f4 | 25 |
| #228 | Game: system, file loading, main.cpp (8/10): 0x8012f900–0x80130264 | 24 |
| #229 | Game: system, file loading, main.cpp (9/10): 0x80130270–0x801311f4 | 25 |
| #230 | Game: system, file loading, main.cpp (10/10): 0x80131200–0x80131780 | 17 |
| #231 | Game: script bindings A (1/14): 0x80131780–0x80131d64 | 21 |
| #232 | Game: script bindings A (2/14): 0x80131d70–0x801325c4 | 25 |
| #233 | Game: script bindings A (3/14): 0x801325d0–0x80133204 | 21 |
| #234 | Game: script bindings A (4/14): 0x80133210–0x80133e94 | 22 |
| #235 | Game: script bindings A (5/14): 0x80133ea0–0x80134bc4 | 25 |
| #236 | Game: script bindings A (6/14): 0x80134bd0–0x801352ec | 27 |
| #237 | Game: script bindings A (7/14): 0x801352f0–0x80136ce4 | 24 |
| #238 | Game: script bindings A (8/14): 0x80136cf0–0x80138ef4 | 28 |
| #239 | Game: script bindings A (9/14): 0x80138f00–0x8013a784 | 32 |
| #240 | Game: script bindings A (10/14): 0x8013a790–0x8013b584 | 19 |
| #241 | Game: script bindings A (11/14): 0x8013b590–0x8013d354 | 35 |
| #242 | Game: script bindings A (12/14): 0x8013d360–0x8013e2e4 | 21 |
| #243 | Game: script bindings A (13/14): 0x8013e2f0–0x8013f284 | 25 |
| #244 | Game: script bindings A (14/14): 0x8013f290–0x80140c40 | 37 |
| #245 | Game: script bindings B (1/5): 0x80140c40–0x80142070 | 17 |
| #246 | Game: script bindings B (2/5): 0x80142070–0x801440a0 | 25 |
| #247 | Game: script bindings B (3/5): 0x801440a0–0x80145a94 | 24 |
| #248 | Game: script bindings B (4/5): 0x80145aa0–0x80146ee0 | 28 |
| #249 | Game: script bindings B (5/5): 0x80146ee0–0x80149520 | 35 |
| #250 | Game: script bindings C (1/26): 0x80149520–0x8014af14 | 24 |
| #251 | Game: script bindings C (2/26): 0x8014af20–0x8014bac4 | 24 |
| #252 | Game: script bindings C (3/26): 0x8014bad0–0x8014cd14 | 25 |
| #253 | Game: script bindings C (4/26): 0x8014cd20–0x8014d754 | 25 |
| #254 | Game: script bindings C (5/26): 0x8014d760–0x8014dcb4 | 22 |
| #255 | Game: script bindings C (6/26): 0x8014dcc0–0x8014e174 | 22 |
| #256 | Game: script bindings C (7/26): 0x8014e180–0x8014e544 | 22 |
| #257 | Game: script bindings C (8/26): 0x8014e550–0x8014f244 | 25 |
| #258 | Game: script bindings C (9/26): 0x8014f250–0x8014fb14 | 27 |
| #259 | Game: script bindings C (10/26): 0x8014fb20–0x80150274 | 27 |
| #260 | Game: script bindings C (11/26): 0x80150280–0x801504a4 | 20 |
| #261 | Game: script bindings C (12/26): 0x801504b0–0x80151124 | 25 |
| #262 | Game: script bindings C (13/26): 0x80151130–0x80152d84 | 27 |
| #263 | Game: script bindings C (14/26): 0x80152d90–0x80153924 | 31 |
| #264 | Game: script bindings C (15/26): 0x80153930–0x80154134 | 27 |
| #265 | Game: script bindings C (16/26): 0x80154140–0x80154c94 | 29 |
| #266 | Game: script bindings C (17/26): 0x80154ca0–0x80155264 | 19 |
| #267 | Game: script bindings C (18/26): 0x80155270–0x80155a84 | 23 |
| #268 | Game: script bindings C (19/26): 0x80155a90–0x80155fd8 | 32 |
| #269 | Game: script bindings C (20/26): 0x80155fe0–0x80157dd4 | 23 |
| #270 | Game: script bindings C (21/26): 0x80157de0–0x80158ff4 | 26 |
| #271 | Game: script bindings C (22/26): 0x80159000–0x8015a0d4 | 23 |
| #272 | Game: script bindings C (23/26): 0x8015a0e0–0x8015b624 | 25 |
| #273 | Game: script bindings C (24/26): 0x8015b630–0x8015c9f4 | 24 |
| #274 | Game: script bindings C (25/26): 0x8015ca00–0x8015e0d4 | 28 |
| #275 | Game: script bindings C (26/26): 0x8015e0e0–0x8015eeb0 | 18 |
| #276 | Game: save data (1/4): 0x8015eeb0–0x801613e4 | 23 |
| #277 | Game: save data (2/4): 0x801613f0–0x80165ad4 | 20 |
| #278 | Game: save data (3/4): 0x80165ae0–0x8016aad4 | 37 |
| #279 | Game: save data (4/4): 0x8016aae0–0x8016cb18 | 20 |
| #280 | Game: photo saving: 0x8016cb20–0x8016fc08 | 6 |
| #281 | Game: NPpu script VM (1/4): 0x8016fc10–0x80170924 | 25 |
| #282 | Game: NPpu script VM (2/4): 0x80170930–0x801738a4 | 27 |
| #283 | Game: NPpu script VM (3/4): 0x801738b0–0x80175804 | 24 |
| #284 | Game: NPpu script VM (4/4): 0x80175810–0x801770f4 | 18 |
| #307 | Game: CFiber, CFileData (1/6): 0x8019dc50–0x801a06d4 | 30 |
| #308 | Game: CFiber, CFileData (2/6): 0x801a06e0–0x801a2f84 | 24 |
| #309 | Game: CFiber, CFileData (3/6): 0x801a2f90–0x801a4e94 | 33 |
| #310 | Game: CFiber, CFileData (4/6): 0x801a4ea0–0x801a9bf8 | 26 |
| #311 | Game: CFiber, CFileData (5/6): 0x801a9c00–0x801aafb4 | 23 |
| #312 | Game: CFiber, CFileData (6/6): 0x801aafc0–0x801adc38 | 25 |
| #313 | Game: CPath, NAND wrappers, memory (1/3): 0x801adc40–0x801afc7c | 35 |
| #314 | Game: CPath, NAND wrappers, memory (2/3): 0x801afc80–0x801b0ee8 | 25 |
| #315 | Game: CPath, NAND wrappers, memory (3/3): 0x801b0ef0–0x801b15f4 | 20 |
| #316 | Game: pad.cpp, reset handling (1/2): 0x801b1600–0x801b4fb4 | 30 |
| #317 | Game: pad.cpp, reset handling (2/2): 0x801b4fc0–0x801b64a8 | 38 |
| #318 | Game: CModel (1/7): 0x801b64b0–0x801b7b64 | 23 |
| #319 | Game: CModel (2/7): 0x801b7b70–0x801b9444 | 26 |
| #320 | Game: CModel (3/7): 0x801b9450–0x801bbb84 | 24 |
| #321 | Game: CModel (4/7): 0x801bbb90–0x801bca44 | 25 |
| #322 | Game: CModel (5/7): 0x801bca50–0x801bef14 | 24 |
| #323 | Game: CModel (6/7): 0x801bef20–0x801c3e34 | 25 |
| #324 | Game: CModel (7/7): 0x801c3e40–0x801c5808 | 39 |
| #325 | Game: G3D extension classes (1/2): 0x801c5810–0x801c6dac | 37 |
| #326 | Game: G3D extension classes (2/2): 0x801c6db0–0x801c8700 | 30 |
| #327 | Game: UI framework and fonts (1/2): 0x801c8700–0x801c9844 | 24 |
| #328 | Game: UI framework and fonts (2/2): 0x801c9850–0x801ca87c | 25 |
| #329 | Game: CLayout.cpp (1/7): 0x801ca880–0x801cca74 | 27 |
| #330 | Game: CLayout.cpp (2/7): 0x801cca80–0x801cf554 | 26 |
| #331 | Game: CLayout.cpp (3/7): 0x801cf560–0x801d0494 | 21 |
| #332 | Game: CLayout.cpp (4/7): 0x801d04a0–0x801d35f4 | 24 |
| #333 | Game: CLayout.cpp (5/7): 0x801d3600–0x801d6994 | 23 |
| #334 | Game: CLayout.cpp (6/7): 0x801d69a0–0x801d87d4 | 25 |
| #335 | Game: CLayout.cpp (7/7): 0x801d87e0–0x801d9a08 | 25 |
| #336 | Game: CRomFont, CUIList, CUIMenu (1/4): 0x801d9a10–0x801db244 | 22 |
| #337 | Game: CRomFont, CUIList, CUIMenu (2/4): 0x801db250–0x801dd344 | 31 |
| #338 | Game: CRomFont, CUIList, CUIMenu (3/4): 0x801dd350–0x801de6b8 | 15 |
| #339 | Game: CRomFont, CUIList, CUIMenu (4/4): 0x801de6c0–0x801df650 | 35 |
| #340 | Game: errmsg.cpp, homebutton.cpp (1/3): 0x801df650–0x801e1bb4 | 28 |
| #341 | Game: errmsg.cpp, homebutton.cpp (2/3): 0x801e1bc0–0x801e3194 | 29 |
| #342 | Game: errmsg.cpp, homebutton.cpp (3/3): 0x801e31a0–0x801e5d98 | 33 |
| #343 | Game: sound (1/2): 0x801e5da0–0x801e8738 | 40 |
| #344 | Game: sound (2/2): 0x801e8740–0x801e8dd8 | 14 |
| #346 | Game: message resources and formatting (1/2): 0x801ee470–0x801f11b8 | 29 |
| #347 | Game: message resources and formatting (2/2): 0x801f11c0–0x801f6060 | 21 |

## RVL_SDK

74 issues, 2,223 functions.

| Issue | Title | Functions |
| --- | --- | --- |
| #286 | RVL_SDK: CNT: 0x801778f0–0x8017852c | 12 |
| #463 | RVL_SDK (1/73): 0x802b1800–0x802b1a10 | 23 |
| #464 | RVL_SDK (2/73): 0x802b1a10–0x802b3a38 | 35 |
| #465 | RVL_SDK (3/73): 0x802b3a40–0x802b46c8 | 37 |
| #466 | RVL_SDK (4/73): 0x802b46d0–0x802b78f8 | 34 |
| #467 | RVL_SDK (5/73): 0x802b7900–0x802b98a0 | 35 |
| #468 | RVL_SDK (6/73): 0x802b98a0–0x802bae2c | 33 |
| #469 | RVL_SDK (7/73): 0x802bae30–0x802bd088 | 40 |
| #470 | RVL_SDK (8/73): 0x802bd090–0x802bf1d8 | 39 |
| #471 | RVL_SDK (9/73): 0x802bf1e0–0x802c23b8 | 39 |
| #472 | RVL_SDK (10/73): 0x802c23c0–0x802c4218 | 31 |
| #473 | RVL_SDK (11/73): 0x802c4220–0x802c7344 | 28 |
| #474 | RVL_SDK (12/73): 0x802c7350–0x802c9908 | 37 |
| #475 | RVL_SDK (13/73): 0x802c9910–0x802cc624 | 35 |
| #476 | RVL_SDK (14/73): 0x802cc630–0x802cd434 | 27 |
| #477 | RVL_SDK (15/73): 0x802cd440–0x802ce624 | 30 |
| #478 | RVL_SDK (16/73): 0x802ce630–0x802cfb84 | 40 |
| #479 | RVL_SDK (17/73): 0x802cfb90–0x802d0a54 | 30 |
| #480 | RVL_SDK (18/73): 0x802d0a60–0x802d1a6c | 25 |
| #481 | RVL_SDK (19/73): 0x802d1a70–0x802d6658 | 50 |
| #482 | RVL_SDK (20/73): 0x802d6660–0x802d782c | 30 |
| #483 | RVL_SDK (21/73): 0x802d7830–0x802d9f24 | 33 |
| #484 | RVL_SDK (22/73): 0x802d9f30–0x802da7bc | 19 |
| #485 | RVL_SDK (23/73): 0x802da7c0–0x802db284 | 24 |
| #486 | RVL_SDK (24/73): 0x802db290–0x802dd2a0 | 35 |
| #487 | RVL_SDK (25/73): 0x802dd2a0–0x802df184 | 25 |
| #488 | RVL_SDK (26/73): 0x802df190–0x802e1898 | 36 |
| #489 | RVL_SDK (27/73): 0x802e18a0–0x802e41bc | 40 |
| #490 | RVL_SDK (28/73): 0x802e41c0–0x802e5bb4 | 33 |
| #491 | RVL_SDK (29/73): 0x802e5bc0–0x802e7c44 | 38 |
| #492 | RVL_SDK (30/73): 0x802e7c50–0x802e9a88 | 39 |
| #493 | RVL_SDK (31/73): 0x802e9a90–0x802ecfa4 | 29 |
| #494 | RVL_SDK (32/73): 0x802ecfb0–0x802edb24 | 27 |
| #495 | RVL_SDK (33/73): 0x802edb30–0x802f08d8 | 29 |
| #496 | RVL_SDK (34/73): 0x802f08e0–0x802f1318 | 21 |
| #497 | RVL_SDK (35/73): 0x802f1320–0x802f342c | 31 |
| #498 | RVL_SDK (36/73): 0x802f3430–0x802f4c40 | 28 |
| #499 | RVL_SDK (37/73): 0x802f4c40–0x802f6bbc | 24 |
| #500 | RVL_SDK (38/73): 0x802f6bc0–0x802f8bfc | 23 |
| #501 | RVL_SDK (39/73): 0x802f8c00–0x802fe620 | 54 |
| #502 | RVL_SDK (40/73): 0x802fe620–0x803039e0 | 31 |
| #503 | RVL_SDK (41/73): 0x803039e0–0x803067f4 | 34 |
| #504 | RVL_SDK (42/73): 0x80306800–0x8030a964 | 40 |
| #505 | RVL_SDK (43/73): 0x8030a970–0x8030d234 | 24 |
| #506 | RVL_SDK (44/73): 0x8030d240–0x8030ff44 | 39 |
| #507 | RVL_SDK (45/73): 0x8030ff50–0x80311314 | 26 |
| #508 | RVL_SDK (46/73): 0x80311320–0x80312584 | 25 |
| #509 | RVL_SDK (47/73): 0x80312590–0x80313a54 | 26 |
| #510 | RVL_SDK (48/73): 0x80313a60–0x80313e34 | 16 |
| #511 | RVL_SDK (49/73): 0x80313e40–0x8031486c | 25 |
| #512 | RVL_SDK (50/73): 0x8031486c–0x8031590c | 25 |
| #513 | RVL_SDK (51/73): 0x8031590c–0x8031694c | 29 |
| #514 | RVL_SDK (52/73): 0x8031694c–0x80317c3c | 25 |
| #515 | RVL_SDK (53/73): 0x80317c3c–0x80319bd4 | 29 |
| #516 | RVL_SDK (54/73): 0x80319bd4–0x8031b73c | 30 |
| #517 | RVL_SDK (55/73): 0x8031b73c–0x8031d914 | 37 |
| #518 | RVL_SDK (56/73): 0x8031d914–0x8031ea7c | 22 |
| #519 | RVL_SDK (57/73): 0x8031ea7c–0x803209f4 | 28 |
| #520 | RVL_SDK (58/73): 0x803209f4–0x803224d0 | 22 |
| #521 | RVL_SDK (59/73): 0x803224d0–0x803255f4 | 32 |
| #522 | RVL_SDK (60/73): 0x803255f4–0x80327880 | 39 |
| #523 | RVL_SDK (61/73): 0x80327880–0x80328878 | 25 |
| #524 | RVL_SDK (62/73): 0x80328878–0x8032a544 | 32 |
| #525 | RVL_SDK (63/73): 0x8032a544–0x8032cf68 | 25 |
| #526 | RVL_SDK (64/73): 0x8032cf68–0x8032f0e0 | 23 |
| #527 | RVL_SDK (65/73): 0x8032f0e0–0x803311f8 | 25 |
| #528 | RVL_SDK (66/73): 0x803311f8–0x803328f8 | 25 |
| #529 | RVL_SDK (67/73): 0x803328f8–0x80333c74 | 17 |
| #530 | RVL_SDK (68/73): 0x80333c74–0x80335464 | 24 |
| #531 | RVL_SDK (69/73): 0x80335464–0x80336ab0 | 25 |
| #532 | RVL_SDK (70/73): 0x80336ab0–0x8033868c | 27 |
| #533 | RVL_SDK (71/73): 0x8033868c–0x8033aa00 | 32 |
| #534 | RVL_SDK (72/73): 0x8033aa00–0x8033e518 | 40 |
| #535 | RVL_SDK (73/73): 0x8033e520–0x8033f974 | 21 |

## NW4R

87 issues, 2,376 functions.

| Issue | Title | Functions |
| --- | --- | --- |
| #375 | NW4R: ef (1/11): 0x80225820–0x80226954 | 25 |
| #376 | NW4R: ef (2/11): 0x80226960–0x80228a14 | 24 |
| #377 | NW4R: ef (3/11): 0x80228a20–0x8022f4f4 | 21 |
| #378 | NW4R: ef (4/11): 0x8022f500–0x80234274 | 25 |
| #379 | NW4R: ef (5/11): 0x80234280–0x80235ca4 | 25 |
| #380 | NW4R: ef (6/11): 0x80235cb0–0x80238264 | 25 |
| #381 | NW4R: ef (7/11): 0x80238270–0x8023e0e4 | 26 |
| #382 | NW4R: ef (8/11): 0x8023e0f0–0x802408b4 | 26 |
| #383 | NW4R: ef (9/11): 0x802408c0–0x80243034 | 24 |
| #384 | NW4R: ef (10/11): 0x80243040–0x80247a64 | 25 |
| #385 | NW4R: ef (11/11): 0x80247a70–0x8024ae80 | 17 |
| #386 | NW4R: g3d (1/30): 0x8024ae80–0x8024b724 | 19 |
| #387 | NW4R: g3d (2/30): 0x8024b730–0x8024c584 | 23 |
| #388 | NW4R: g3d (3/30): 0x8024c590–0x8024d704 | 24 |
| #389 | NW4R: g3d (4/30): 0x8024d710–0x8024f084 | 28 |
| #390 | NW4R: g3d (5/30): 0x8024f090–0x8024f764 | 26 |
| #391 | NW4R: g3d (6/30): 0x8024f770–0x802507d4 | 24 |
| #392 | NW4R: g3d (7/30): 0x802507e0–0x80251114 | 26 |
| #393 | NW4R: g3d (8/30): 0x80251120–0x80253a84 | 28 |
| #394 | NW4R: g3d (9/30): 0x80253a90–0x80255504 | 31 |
| #395 | NW4R: g3d (10/30): 0x80255510–0x80255e74 | 22 |
| #396 | NW4R: g3d (11/30): 0x80255e80–0x80256af4 | 23 |
| #397 | NW4R: g3d (12/30): 0x80256b00–0x80257594 | 23 |
| #398 | NW4R: g3d (13/30): 0x802575a0–0x80258084 | 24 |
| #399 | NW4R: g3d (14/30): 0x80258090–0x80258d44 | 26 |
| #400 | NW4R: g3d (15/30): 0x80258d50–0x80259734 | 24 |
| #401 | NW4R: g3d (16/30): 0x80259740–0x8025a244 | 26 |
| #402 | NW4R: g3d (17/30): 0x8025a250–0x8025b694 | 37 |
| #403 | NW4R: g3d (18/30): 0x8025b6a0–0x8025dd84 | 23 |
| #404 | NW4R: g3d (19/30): 0x8025dd90–0x8025f9f4 | 21 |
| #405 | NW4R: g3d (20/30): 0x8025fa00–0x80263274 | 24 |
| #406 | NW4R: g3d (21/30): 0x80263280–0x802650d4 | 26 |
| #407 | NW4R: g3d (22/30): 0x802650e0–0x802665f4 | 22 |
| #408 | NW4R: g3d (23/30): 0x80266600–0x802677c4 | 25 |
| #409 | NW4R: g3d (24/30): 0x802677d0–0x802685b4 | 25 |
| #410 | NW4R: g3d (25/30): 0x802685c0–0x80269874 | 23 |
| #411 | NW4R: g3d (26/30): 0x80269880–0x8026ad34 | 24 |
| #412 | NW4R: g3d (27/30): 0x8026ad40–0x8026cb74 | 25 |
| #413 | NW4R: g3d (28/30): 0x8026cb80–0x8026f344 | 25 |
| #414 | NW4R: g3d (29/30): 0x8026f350–0x80270644 | 25 |
| #415 | NW4R: g3d (30/30): 0x80270650–0x80271e0c | 35 |
| #416 | NW4R: snd (1/28): 0x80271e10–0x80273694 | 24 |
| #417 | NW4R: snd (2/28): 0x802736a0–0x80275874 | 26 |
| #418 | NW4R: snd (3/28): 0x80275880–0x80276f94 | 21 |
| #419 | NW4R: snd (4/28): 0x80276fa0–0x80278374 | 26 |
| #420 | NW4R: snd (5/28): 0x80278380–0x80279244 | 25 |
| #421 | NW4R: snd (6/28): 0x80279250–0x8027a414 | 27 |
| #422 | NW4R: snd (7/28): 0x8027a420–0x8027b3f4 | 29 |
| #423 | NW4R: snd (8/28): 0x8027b400–0x8027c044 | 25 |
| #424 | NW4R: snd (9/28): 0x8027c050–0x8027cbf4 | 22 |
| #425 | NW4R: snd (10/28): 0x8027cc00–0x8027e054 | 30 |
| #426 | NW4R: snd (11/28): 0x8027e060–0x8027e6f4 | 29 |
| #427 | NW4R: snd (12/28): 0x8027e700–0x8027f924 | 20 |
| #428 | NW4R: snd (13/28): 0x8027f930–0x80280a84 | 26 |
| #429 | NW4R: snd (14/28): 0x80280a90–0x80281154 | 21 |
| #430 | NW4R: snd (15/28): 0x80281160–0x80282cb4 | 25 |
| #431 | NW4R: snd (16/28): 0x80282cc0–0x80283e94 | 39 |
| #432 | NW4R: snd (17/28): 0x80283ea0–0x802847b4 | 28 |
| #433 | NW4R: snd (18/28): 0x802847c0–0x802858e4 | 26 |
| #435 | NW4R: snd (19/28): 0x802858f0–0x80287f44 | 30 |
| #436 | NW4R: snd (20/28): 0x80287f50–0x80289014 | 25 |
| #437 | NW4R: snd (21/28): 0x80289020–0x8028a924 | 26 |
| #438 | NW4R: snd (22/28): 0x8028a930–0x8028c6e4 | 22 |
| #439 | NW4R: snd (23/28): 0x8028c6f0–0x8028d0e4 | 25 |
| #440 | NW4R: snd (24/28): 0x8028d0f0–0x8028d994 | 21 |
| #441 | NW4R: snd (25/28): 0x8028d9a0–0x8028efb4 | 38 |
| #442 | NW4R: snd (26/28): 0x8028efc0–0x80290d64 | 24 |
| #443 | NW4R: snd (27/28): 0x80290d70–0x80291774 | 22 |
| #444 | NW4R: snd (28/28): 0x80291780–0x80292cf4 | 37 |
| #445 | NW4R: ut, lyt, math, db (1/18): 0x80292d00–0x80293cc4 | 36 |
| #446 | NW4R: ut, lyt, math, db (2/18): 0x80293cd0–0x80294594 | 27 |
| #447 | NW4R: ut, lyt, math, db (3/18): 0x802945a0–0x80294c74 | 26 |
| #448 | NW4R: ut, lyt, math, db (4/18): 0x80294c80–0x80295464 | 27 |
| #449 | NW4R: ut, lyt, math, db (5/18): 0x80295470–0x80297594 | 37 |
| #450 | NW4R: ut, lyt, math, db (6/18): 0x802975a0–0x8029a16c | 36 |
| #451 | NW4R: ut, lyt, math, db (7/18): 0x8029a170–0x8029c31c | 27 |
| #452 | NW4R: ut, lyt, math, db (8/18): 0x8029c320–0x802a1944 | 56 |
| #453 | NW4R: ut, lyt, math, db (9/18): 0x802a1950–0x802a2534 | 23 |
| #454 | NW4R: ut, lyt, math, db (10/18): 0x802a2540–0x802a2f74 | 27 |
| #455 | NW4R: ut, lyt, math, db (11/18): 0x802a2f80–0x802a4bb4 | 36 |
| #456 | NW4R: ut, lyt, math, db (12/18): 0x802a4bc0–0x802a5840 | 23 |
| #457 | NW4R: ut, lyt, math, db (13/18): 0x802a5840–0x802a6944 | 26 |
| #458 | NW4R: ut, lyt, math, db (14/18): 0x802a6950–0x802a8098 | 31 |
| #459 | NW4R: ut, lyt, math, db (15/18): 0x802a80a0–0x802abd4c | 38 |
| #460 | NW4R: ut, lyt, math, db (16/18): 0x802abd50–0x802aeb30 | 77 |
| #461 | NW4R: ut, lyt, math, db (17/18): 0x802aeb30–0x802b08f0 | 40 |
| #462 | NW4R: ut, lyt, math, db (18/18): 0x802b08f0–0x802b1800 | 24 |

## HBM

20 issues, 559 functions.

| Issue | Title | Functions |
| --- | --- | --- |
| #287 | HBM: homebutton core (1/6): 0x80178530–0x80179124 | 25 |
| #288 | HBM: homebutton core (2/6): 0x80179130–0x80179d44 | 23 |
| #289 | HBM: homebutton core (3/6): 0x80179d50–0x8017a6e4 | 26 |
| #290 | HBM: homebutton core (4/6): 0x8017a6f0–0x8017b394 | 23 |
| #291 | HBM: homebutton core (5/6): 0x8017b3a0–0x8017ca8c | 34 |
| #292 | HBM: homebutton core (6/6): 0x8017ca90–0x80186334 | 33 |
| #293 | HBM: nw4hbm::lyt (1/6): 0x80186340–0x80187300 | 18 |
| #294 | HBM: nw4hbm::lyt (2/6): 0x80187300–0x801890a8 | 39 |
| #295 | HBM: nw4hbm::lyt (3/6): 0x801890b0–0x8018c614 | 28 |
| #296 | HBM: nw4hbm::lyt (4/6): 0x8018c620–0x8018dc14 | 40 |
| #297 | HBM: nw4hbm::lyt (5/6): 0x8018dc20–0x8018f884 | 30 |
| #298 | HBM: nw4hbm::lyt (6/6): 0x8018f890–0x80191874 | 17 |
| #299 | HBM: nw4hbm::ut (1/6): 0x80191880–0x80192a54 | 14 |
| #300 | HBM: nw4hbm::ut (2/6): 0x80192a60–0x80193d4c | 34 |
| #301 | HBM: nw4hbm::ut (3/6): 0x80193d50–0x80194398 | 24 |
| #302 | HBM: nw4hbm::ut (4/6): 0x801943a0–0x80196394 | 39 |
| #303 | HBM: nw4hbm::ut (5/6): 0x801963a0–0x80197a78 | 25 |
| #304 | HBM: nw4hbm::ut (6/6): 0x80197a80–0x8019a2c4 | 31 |
| #305 | HBM: sound (1/2): 0x8019a2d0–0x8019c9c4 | 32 |
| #306 | HBM: sound (2/2): 0x8019c9d0–0x8019dc48 | 24 |

## Middleware

28 issues, 690 functions.

| Issue | Title | Functions |
| --- | --- | --- |
| #285 | Middleware: RevoEX (SHA-1) and NCD: 0x801770f4–0x801778e4 | 8 |
| #348 | Middleware: TMC JPEG (1/2): 0x801f6060–0x801f8dc4 | 25 |
| #349 | Middleware: TMC JPEG (2/2): 0x801f8dc4–0x801fb030 | 25 |
| #350 | Middleware: FA file system (1/25): 0x801fb030–0x801fb9c4 | 25 |
| #351 | Middleware: FA file system (2/25): 0x801fb9d0–0x801fc5a4 | 22 |
| #352 | Middleware: FA file system (3/25): 0x801fc5b0–0x801fd5c4 | 24 |
| #353 | Middleware: FA file system (4/25): 0x801fd5d0–0x801ff004 | 26 |
| #354 | Middleware: FA file system (5/25): 0x801ff010–0x801ffd84 | 25 |
| #355 | Middleware: FA file system (6/25): 0x801ffd90–0x80204344 | 25 |
| #356 | Middleware: FA file system (7/25): 0x80204350–0x80206114 | 26 |
| #357 | Middleware: FA file system (8/25): 0x80206120–0x80208614 | 22 |
| #358 | Middleware: FA file system (9/25): 0x80208620–0x8020b2d4 | 29 |
| #359 | Middleware: FA file system (10/25): 0x8020b2e0–0x8020d0d4 | 23 |
| #360 | Middleware: FA file system (11/25): 0x8020d0e0–0x8020f874 | 25 |
| #361 | Middleware: FA file system (12/25): 0x8020f880–0x80211d04 | 25 |
| #362 | Middleware: FA file system (13/25): 0x80211d10–0x80214954 | 26 |
| #363 | Middleware: FA file system (14/25): 0x80214960–0x80216bf4 | 23 |
| #364 | Middleware: FA file system (15/25): 0x80216c00–0x8021a1d4 | 26 |
| #365 | Middleware: FA file system (16/25): 0x8021a1e0–0x8021aa94 | 26 |
| #366 | Middleware: FA file system (17/25): 0x8021aaa0–0x8021b184 | 26 |
| #367 | Middleware: FA file system (18/25): 0x8021b190–0x8021b884 | 25 |
| #368 | Middleware: FA file system (19/25): 0x8021b890–0x8021c344 | 25 |
| #369 | Middleware: FA file system (20/25): 0x8021c350–0x8021d1f4 | 22 |
| #370 | Middleware: FA file system (21/25): 0x8021d200–0x8021ddc4 | 28 |
| #371 | Middleware: FA file system (22/25): 0x8021ddd0–0x8021f3a4 | 28 |
| #372 | Middleware: FA file system (23/25): 0x8021f3b0–0x80220884 | 17 |
| #373 | Middleware: FA file system (24/25): 0x80220890–0x80223774 | 28 |
| #374 | Middleware: FA file system (25/25): 0x80223780–0x80225820 | 35 |

## MSL_C and runtime

12 issues, 351 functions.

| Issue | Title | Functions |
| --- | --- | --- |
| #37 | Init section: startup and runtime memory routines: 0x80004000–0x800046e4 | 11 |
| #345 | MSL_C: printf and wide printf: 0x801e8de0–0x801ee468 | 29 |
| #536 | MSL_C and runtime (1/10): 0x8033f974–0x80340b24 | 31 |
| #537 | MSL_C and runtime (2/10): 0x80340b94–0x80344eb4 | 40 |
| #538 | MSL_C and runtime (3/10): 0x80344eb4–0x80347904 | 40 |
| #539 | MSL_C and runtime (4/10): 0x80347904–0x8034d184 | 40 |
| #540 | MSL_C and runtime (5/10): 0x8034d184–0x8034d83c | 15 |
| #541 | MSL_C and runtime (6/10): 0x8034d83c–0x8034e37c | 32 |
| #542 | MSL_C and runtime (7/10): 0x8034e380–0x8034ebd4 | 25 |
| #543 | MSL_C and runtime (8/10): 0x8034ebd4–0x8034f9cc | 25 |
| #544 | MSL_C and runtime (9/10): 0x8034f9cc–0x803510d4 | 25 |
| #545 | MSL_C and runtime (10/10): 0x803510d4–0x803529bc | 38 |
