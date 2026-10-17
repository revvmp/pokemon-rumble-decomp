# Issue 288: HBM homebutton core, block 2

## F-288-1: the whole block is HBMGUIManager.cpp

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). All 23 functions of this block (`0x80179130`–`0x80179D44`) belong to `RVL_SDK/hbm/HBMGUIManager.cpp`, whose first function is in #287's block. #287 split that unit (`.text` `0x801787F0`–`0x8017A008`) and matched it in pull request #606; see [F-287-3](287-hbm-core.md#f-287-3-hbmguimanagercpp-is-one-unit-from-0x801787f0-to-0x8017a008) for boundaries, class layouts and evidence.

On `main` after #606, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit `Matching`, and objdiff 3.8.2 reports each function below at 100%:

| Address | Size | Name |
| --- | --- | --- |
| `0x80179130` | `0x8` | `isTriggerTarger__Q310homebutton3gui9ComponentFv` |
| `0x80179140` | `0x30` | `onTrig__Q310homebutton3gui9ComponentFUlR3Vec` |
| `0x80179170` | `0x68` | `calc__Q310homebutton3gui7ManagerFv` |
| `0x801791E0` | `0x4` | `calc__Q310homebutton3gui9InterfaceFv` |
| `0x801791F0` | `0x68` | `draw__Q310homebutton3gui7ManagerFv` |
| `0x80179260` | `0x4` | `draw__Q310homebutton3gui9InterfaceFv` |
| `0x80179270` | `0x78` | `setAllComponentTriggerTarget__Q310homebutton3gui7ManagerFb` |
| `0x801792F0` | `0x8` | `setTriggerTarget__Q310homebutton3gui9ComponentFb` |
| `0x80179300` | `0x154` | `__dt__Q310homebutton3gui11PaneManagerFv` |
| `0x80179460` | `0x40` | `__dt__Q310homebutton3gui13PaneComponentFv` |
| `0x801794A0` | `0x40` | `__dt__Q310homebutton3gui9ComponentFv` |
| `0x801794E0` | `0x24` | `createLayoutScene__Q310homebutton3gui11PaneManagerFRCQ36nw4hbm3lyt6Layout` |
| `0x80179510` | `0x18` | `addLayoutScene__Q310homebutton3gui11PaneManagerFRCQ36nw4hbm3lyt6Layout` |
| `0x80179530` | `0x298` | `walkInChildren__Q310homebutton3gui11PaneManagerFRQ36nw4hbm2ut30LinkList<Q36nw4hbm3lyt4Pane,4>` |
| `0x801797D0` | `0x8` | `setPane__Q310homebutton3gui13PaneComponentFPQ36nw4hbm3lyt4Pane` |
| `0x801797E0` | `0xC` | `GetRuntimeTypeInfo__Q36nw4hbm3lyt4PaneCFv` |
| `0x801797F0` | `0x18` | `delLayoutScene__Q310homebutton3gui11PaneManagerFRCQ36nw4hbm3lyt6Layout` |
| `0x80179810` | `0x11C` | `walkInChildrenDel__Q310homebutton3gui11PaneManagerFRQ36nw4hbm2ut30LinkList<Q36nw4hbm3lyt4Pane,4>` |
| `0x80179930` | `0x78` | `getPaneComponentByPane__Q310homebutton3gui11PaneManagerFPQ36nw4hbm3lyt4Pane` |
| `0x801799B0` | `0xD0` | `setAllBoundingBoxComponentTriggerTarget__Q310homebutton3gui11PaneManagerFb` |
| `0x80179A80` | `0x12C` | `contain__Q310homebutton3gui13PaneComponentFff` |
| `0x80179BB0` | `0x8` | `getDrawInfo__Q310homebutton3gui11PaneManagerFv` |
| `0x80179BC0` | `0x184` | `draw__Q310homebutton3gui13PaneComponentFv` |

No source, split or symbol changes were needed for this block.
