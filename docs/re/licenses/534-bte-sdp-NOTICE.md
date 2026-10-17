# BTE SDP source provenance for issue 534

The reconstruction in `src/RVL_SDK/bte/sdp_main.c`, `sdp_server.c`, `sdp_utils.c`, `sdp_api.c`, `sdp_db.c`, `sdp_discovery.c`, `rfc_ts_frames.c`, `rfc_utils.c`, `rfc_port_fsm.c`, `rfc_port_if.c`, `rfc_l2cap_if.c`, `rfc_mx_fsm.c` and `port_utils.c` (issues 534, 533, 532, 531, 530 and 529), the private headers `include/revolution/bte/sdp_int.h`, `rfc_int.h`, `port_int.h`, `port_api.h` and `rfcdefs.h`, and the SDP, RFCOMM, L2CAP, trace and stream declarations added for them under `include/revolution/bte/`, adapts reference code, names, and structure layouts from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte/`). Changes remove source comments and the variadic trace macro system, drop dead-stripped functions, reduce headers to the declarations these units use, define the SDP status codes as macros, and rename include paths. The files are modified from the reference.

The referenced files carry these notices:

- `sdp_main.c`, `sdp_server.c`, `sdp_utils.c`, `sdp_api.c`, `sdp_db.c`, `sdp_discovery.c`, `rfc_ts_frames.c`, `rfc_utils.c`, `rfc_port_fsm.c`, `rfc_port_if.c`, `rfc_l2cap_if.c`, `rfc_mx_fsm.c`, `port_utils.c`, `sdp_int.h`, `rfc_int.h`, `port_int.h`, `port_api.h`, `rfcdefs.h` and the other bluedroid-derived headers: Copyright (C) 1999-2012 Broadcom Corporation.
- Includes changes by muff1n1634.

Licensed under the Apache License, Version 2.0 (the License); you may not use this file except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The complete license is [preserved here](510-apache-2.0.txt). The original BTE NOTICE is reproduced in [the issue 510 notice](510-bte-NOTICE.md).
