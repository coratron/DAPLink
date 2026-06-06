/**
 * @file    s32k3_reset.c
 * @brief   Target family descriptor for the NXP S32K3 series (Cortex-M7).
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

#include <stdbool.h>

#include "swd_host.h"
#include "target_family.h"

/* AP indices on the S32K3 SWD bus (confirmed from S32K3xx_DFP + pyOCD). */
#define S32K3_MDM_AP                        6U      /* MDM-AP            */
#define S32K3_SDA_AP                        7U      /* SDA-AP            */
#define S32K3_CM7_0_AHB_AP                  4U      /* AHB-AP for CM7_0  */

/* NXP custom debug-control AP registers (in their own AP banks). */
#define S32K3_MDMAPCTL                      0x04U   /* MDM-AP            */
#define S32K3_SDA_AP_DBGENCTRL              0x80U   /* SDA-AP, bank 8    */
#define S32K3_SDA_AP_RSTCTRL                0x90U   /* SDA-AP, bank 9    */

#define S32K3_SDA_AP_DBGENCTRL_ENABLE_M7    0x300000F0U
#define S32K3_MDMAPCTL_ENABLE_CM7_0_ACCESS  0x00430000U
#define S32K3_SDA_AP_RSTCTRL_HOLD_CM7_0     0x00000000U
#define S32K3_SDA_AP_RSTCTRL_RELEASE_CM7_0  0x02000000U

/*
 * Write an AP register on an EXPLICIT AP, bypassing swd_get_apsel().
 *
 * The family .apsel below is pinned to the CM7_0 AHB-AP (#4) so that DAPLink's
 * generic halt + flash memory accesses reach the core. But swd_write_ap() runs
 * every access through swd_get_apsel(), which then forces APSEL #4 for all
 * transfers. The SDA-AP (#7) / MDM-AP (#6) debug-control registers we need for
 * the unlock live on different APs, so we drive DP SELECT + the AP transfer
 * directly here.
 */
static bool s32k3_write_ap_raw(uint8_t apsel, uint32_t reg, uint32_t value)
{
    uint32_t sel = ((uint32_t)apsel << 24) | (reg & APBANKSEL);
    if (!swd_write_dp(DP_SELECT, sel)) {
        return false;
    }

    uint32_t req = SWD_REG_AP | SWD_REG_W | SWD_REG_ADR(reg);
    if (swd_transfer_retry(req, &value) != 0x01U) {
        return false;
    }

    /* Read RDBUFF to complete the posted AP write. */
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    (void)swd_transfer_retry(req, NULL);
    return true;
}

/*
 * Enable M7 debug and release CM7_0 from reset. Called from swd_init_debug()
 * (after DP power-up, before any AHB-AP access), so the core AHB-AP is live by
 * the time the generic reset/halt/flash path runs against APSEL #4.
 */
static uint8_t s32k3_target_unlock_sequence(void)
{
    if (!s32k3_write_ap_raw(S32K3_SDA_AP, S32K3_SDA_AP_DBGENCTRL, S32K3_SDA_AP_DBGENCTRL_ENABLE_M7)) {
        return 0U;
    }
    if (!s32k3_write_ap_raw(S32K3_MDM_AP, S32K3_MDMAPCTL, S32K3_MDMAPCTL_ENABLE_CM7_0_ACCESS)) {
        return 0U;
    }
    if (!s32k3_write_ap_raw(S32K3_SDA_AP, S32K3_SDA_AP_RSTCTRL, S32K3_SDA_AP_RSTCTRL_HOLD_CM7_0)) {
        return 0U;
    }
    if (!s32k3_write_ap_raw(S32K3_SDA_AP, S32K3_SDA_AP_RSTCTRL, S32K3_SDA_AP_RSTCTRL_RELEASE_CM7_0)) {
        return 0U;
    }
    return 1U;
}

const target_family_descriptor_t g_nxp_s32k3_family = {
    .family_id              = kNXP_S32K3_FamilyID,
    /* Software (SYSRESETREQ) reset, not hardware nRESET: an nRESET wipes the
     * SDA-AP debug-enable we set in the unlock and re-latches the core via the
     * boot ROM, so the subsequent halt fails. A software reset leaves the debug
     * power domain (and our CM7_0 release) intact. */
    .default_reset_type     = kSoftwareReset,
    .soft_reset_type        = SYSRESETREQ,
    .target_unlock_sequence = s32k3_target_unlock_sequence,
    /* No custom target_set_state: the generic swd_set_target_state_hw() halts
     * and flashes through this AP, which must be the CM7_0 AHB-AP (#4). */
    .apsel                  = ((uint32_t)S32K3_CM7_0_AHB_AP << 24),
};
