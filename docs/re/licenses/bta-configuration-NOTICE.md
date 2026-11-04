# BTE configuration source provenance

`src/RVL_SDK/bte/bta_dm_cfg.c`, `bta_hh_cfg.c`, and `bta_sys_cfg.c` adapt the same files from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`. Their primary upstream is Android Bluedroid's `bta/dm/bta_dm_cfg.c`, `bta/hh/bta_hh_cfg.c`, and `bta/sys/bta_sys_cfg.c`. These files are modified: comments move here, includes use project paths, target constants are explicit, and the unlinked p_bta_dm_cfg pointer is omitted. Every retained field and pointer is verified against this target.

File-specific notices supersede Petari's root CC0 license:

- bta_dm_cfg.c and bta_sys_cfg.c: Copyright (C) 2003-2012 Broadcom Corporation.
- bta_hh_cfg.c: Copyright (C) 2005-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use these files except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt); the original BTE NOTICE is reproduced in [the BTE notice](510-bte-NOTICE.md).
