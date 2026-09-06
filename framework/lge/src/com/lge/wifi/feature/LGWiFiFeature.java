/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.wifi.feature;

/**
 * A WLAN feature flag, one singleton per flag, same shape as
 * com.lge.lgdata.LGDataRuntimeFeature. None are on here.
 */
public class LGWiFiFeature {

    public static final LGWiFiFeature LGP_WIFI_HW_KCC_144CH = new LGWiFiFeature();

    public boolean isEnabled() {
        return false;
    }
}
