/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core;

/**
 * FM radio state, which the TV app checks so the two do not fight over the
 * audio path. There is no FM service here, so it always reports off.
 */
public class LgFmManager {

    /** Value LG uses for "the FM radio is not running". */
    public static final int FM_STATE_OFF = 0;

    private static final LgFmManager sInstance = new LgFmManager();

    public static LgFmManager getInstance() {
        return sInstance;
    }

    public int fmServiceGetFMState() {
        return FM_STATE_OFF;
    }
}
