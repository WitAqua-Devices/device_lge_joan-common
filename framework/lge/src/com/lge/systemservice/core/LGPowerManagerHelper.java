/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core;

/**
 * Reached through LGContext.getLGSystemService(). Thermald is LG's thermal
 * daemon, which does not run here, so turning it off is a no-op - callers only
 * use it to keep the panel bright during playback.
 */
public class LGPowerManagerHelper {

    /**
     * Drives the second display on the V30's extended-LCD panels. joan's panel
     * has none, and the hidden menu calls this unconditionally.
     */
    public void changeDisplayModeForExtendedLcd(boolean enabled) {
    }

    public void turnOffThermald() {
    }
}
