/**
 * @file    s32k312minievb.c
 * @brief   board ID for the NXP S32K312-MINI-EVB (onboard K26 OpenSDA),
 *          true S32K312 target (memory map + flash algorithm).
 *
 * DAPLink Interface Firmware
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "target_board.h"
#include "target_family.h"

const board_info_t g_board_info = {
    .info_version       = kBoardInfoVersion,
    .board_id           = "0C00",                 // project-local id; not an mbed-registered board
    .family_id          = kNXP_S32K3_FamilyID,
    .flags              = kEnablePageErase,
    .daplink_url_name   = "PRODINFOHTM",
    .daplink_drive_name = "K312EVB    ",
    .daplink_target_url = "https://www.nxp.com/S32K312-MINI-EVB",
    .target_cfg         = &target_device,
    .board_vendor       = "NXP",
    .board_name         = "S32K312-MINI-EVB",
};
