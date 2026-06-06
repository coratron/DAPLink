/**
 * @file    target.c
 * @brief   Target information for the NXP S32K312 (Cortex-M7).
 *
 * Memory map from NXP S32K3xx_DFP 1.0.1; PFlash geometry from the
 * S32K3x2_Code.FLM flash algorithm (see flash_blob.c).
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

#include "target_config.h"

// The file flash_blob.c must only be included in target.c
#include "flash_blob.c"

// target information
target_cfg_t target_device = {
    .version                     = kTargetConfigVersion,
    .sectors_info                = sectors_info,
    .sector_info_length          = (sizeof(sectors_info)) / (sizeof(sector_info_t)),
    .flash_regions[0].start      = 0x00400000,                   // PFlash
    .flash_regions[0].end        = 0x00400000 + 0x001E0000,      // 0x1E0000 = FLM-reported code-flash size
    .flash_regions[0].flags      = kRegionIsDefault,
    .flash_regions[0].flash_algo = (program_target_t *) &flash,  // generated struct name in flash_blob.c
    .ram_regions[0].start        = 0x20400000,                   // SRAM_0 (flash-algo load region)
    .ram_regions[0].end          = 0x20400000 + KB(96),
    .ram_regions[1].start        = 0x20000000,                   // DTCM
    .ram_regions[1].end          = 0x20000000 + KB(64),
    .target_vendor               = "NXP",
    .target_part_number          = "S32K312",
};
