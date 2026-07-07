/**
 * @file    k26f_bl.c
 * @brief   board ID and meta-data for the hardware interface circuit (HIC) based on the NXP K26F
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdbool.h>

#include "target_config.h"
#include "daplink_addr.h"
#include "compiler.h"
#include "target_board.h"
#include "target_family.h"

// Warning - changing the interface start will break backwards compatibility
COMPILER_ASSERT(DAPLINK_ROM_IF_START == KB(64));

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {DAPLINK_ROM_IF_START, DAPLINK_SECTOR_SIZE},
 };

// k26f target information
target_cfg_t target_device = {
    .version                    = kTargetConfigVersion,
    .sectors_info               = sectors_info,
    .sector_info_length         = (sizeof(sectors_info))/(sizeof(sector_info_t)),
    .flash_regions[0].start     = DAPLINK_ROM_IF_START,
    .flash_regions[0].end       = DAPLINK_ROM_IF_START + DAPLINK_ROM_IF_SIZE,
    .flash_regions[0].flags     = kRegionIsDefault,
    .ram_regions[0].start       = 0x1fff0000,
    .ram_regions[0].end         = 0x20030000,
    /* .flash_algo not needed for bootloader */
};

//bootloader has no family
const target_family_descriptor_t *g_target_family = NULL;

const board_info_t g_board_info = {
    .info_version = kBoardInfoVersion,
    .board_id = "0000",
    .daplink_url_name =   "HELP_FAQHTM",
    .daplink_drive_name = "BOOTLOADER ",
    .daplink_target_url = "https://daplink.io",
    .target_cfg = &target_device,
};

#ifdef BOARD_BL_IGNORE_RESET_BTN
// Strong override of the __WEAK reset_button_pressed() in main_bootloader.c.
//
// On the S32K312-MINI-EVB there is no K26 reset button: the k26f HIC's
// PIN_nRESET sense (PTA7) is wired only to the shared target RESET_b net. A
// target that holds RESET_b low (reset loop, destructive/FCCU reset, blank or
// bricked part) is therefore read as a held reset button, latching the K26 in
// the bootloader. That makes the debug probe unavailable at precisely the
// moment the target is broken and the probe is needed most. Break that coupling
// by not treating the reset line as a bootloader-entry request on this board.
//
// The remaining bootloader-entry paths are unaffected: an invalid interface
// image (validate_bin_nvic) and the software hold flag
// (config_ram_get_initial_hold_in_bl(), set via the START_BL.ACT knock) both
// still hold in the bootloader, and external SWD remains the recovery of last
// resort. Gated per-build like BOARD_SWDIO_BIDIR so the shared k26f bootloader
// keeps stock behaviour for boards that do have a reset button (e.g.
// FRDM-K32L3A6).
bool reset_button_pressed(void)
{
    return false;
}
#endif
