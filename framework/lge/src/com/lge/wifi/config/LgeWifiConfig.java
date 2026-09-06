/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.wifi.config;

/**
 * Build-time answers about which WLAN vendor the device carries. joan is
 * qualcomm, and the version strings come out of LG's own wpa_supplicant
 * additions, which are not here - the hidden menu shows them as unknown.
 */
public class LgeWifiConfig {

    public static final boolean CONFIG_LGE_WLAN_QCOM_PATCH = true;
    public static final boolean CONFIG_LGE_WLAN_BRCM_PATCH = false;
    public static final boolean CONFIG_LGE_WLAN_MTK_PATCH = false;

    public static String getOperator() {
        return "";
    }

    public static String getWlanChipsetVersion() {
        return "";
    }

    public static String getWlanQcaChipsetVersion() {
        return "";
    }

    public static String getWlanHeliumChipsetVersion() {
        return "";
    }

    public static boolean useLgeKtCm() {
        return false;
    }
}
