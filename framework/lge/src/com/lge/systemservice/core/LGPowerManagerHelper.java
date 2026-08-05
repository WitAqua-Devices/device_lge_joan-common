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

    public void turnOffThermald() {
    }
}
