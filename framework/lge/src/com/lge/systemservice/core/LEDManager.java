/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core;

/**
 * LG's notification LED service, reached through
 * LGContext.getLGSystemService("emotionled"). joan drives its LEDs through the
 * lights HAL like any other device here, so there is nothing behind this and
 * the pattern requests are dropped.
 */
public class LEDManager {

    public int startPattern(String owner, int patternId, LGLedRecord record) {
        return -1;
    }

    public int stopPattern(String owner, int patternId) {
        return -1;
    }
}
