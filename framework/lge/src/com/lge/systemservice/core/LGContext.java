/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core;

import com.lge.systemservice.core.halinterface.ChargerHalManager;
import com.lge.systemservice.core.halinterface.WifiLgeHalManager;

/**
 * Entry point to LG's own system services.
 *
 * This used to answer null for everything, which is what a service that failed
 * to start looks like - but the callers are not written for that. They cast the
 * result and go straight on to use it, so a null took the hidden menu's test
 * screens down with a NullPointerException the moment one was opened. Handing
 * back the stub instead means the call lands somewhere that does nothing and
 * says so, which is what the rest of these classes are for.
 *
 * Names are the ones the hidden menu asks for; anything else still gets null,
 * since inventing a stub for a service nobody here names would be guessing.
 */
public class LGContext {

    public static final String LED_SERVICE = "emotionled";
    public static final String VOLUME_VIBRATOR_SERVICE = "volumevibrator";
    public static final String LG_POWER_MANAGER_HELPER_SERVICE = "lgpowermanagerhelper";
    public static final String WIFI_LGE_EXT_SERVICE = "wifiLgeExtService";
    public static final String WIFI_LGE_HAL_MANAGER_SERVICE = "wifiLgeHalManagerService";
    public static final String CHARGER_HAL_SERVICE = "chargerhal";

    private static final LEDManager sLedManager = new LEDManager();
    private static final VolumeVibratorManager sVolumeVibratorManager =
            new VolumeVibratorManager();
    private static final LGPowerManagerHelper sPowerManagerHelper =
            new LGPowerManagerHelper();
    private static final WifiLgeExtManager sWifiLgeExtManager = new WifiLgeExtManager();
    private static final WifiLgeHalManager sWifiLgeHalManager = new WifiLgeHalManager();
    private static final ChargerHalManager sChargerHalManager = new ChargerHalManager();

    public LGContext(android.content.Context context) {
    }

    public Object getLGSystemService(String name) {
        if (name == null) {
            return null;
        }
        switch (name) {
            case LED_SERVICE:
                return sLedManager;
            case VOLUME_VIBRATOR_SERVICE:
                return sVolumeVibratorManager;
            case LG_POWER_MANAGER_HELPER_SERVICE:
                return sPowerManagerHelper;
            case WIFI_LGE_EXT_SERVICE:
                return sWifiLgeExtManager;
            case WIFI_LGE_HAL_MANAGER_SERVICE:
                return sWifiLgeHalManager;
            case CHARGER_HAL_SERVICE:
                return sChargerHalManager;
            default:
                return null;
        }
    }
}
