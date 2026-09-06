/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core;

/**
 * LG's vibrator wrapper, reached through
 * LGContext.getLGSystemService("volumevibrator"). The requests go nowhere; the
 * platform Vibrator is what actually drives the motor on this build.
 */
public class VolumeVibratorManager {

    public void vibrate(long[] pattern, int repeat, int[] amplitudes) {
    }

    public void cancel() {
    }
}
