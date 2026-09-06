/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.mme;

import android.content.Context;

/**
 * Magnetic secure transmission - the coil LG Pay drove to fake a card swipe.
 * The hardware is not described anywhere on this build, so isEnabled is what
 * callers check first and it keeps them out of the rest.
 */
public class MMEManager {

    public static boolean isEnabled = false;

    public MMEManager(Context context) {
    }

    public int initialize() {
        return -1;
    }

    public int setData(int data) {
        return -1;
    }

    public byte[] getData(int type, int index) {
        return new byte[0];
    }

    public int terminate(boolean force) {
        return -1;
    }
}
