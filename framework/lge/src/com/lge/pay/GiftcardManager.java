/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.pay;

import android.content.Context;

/**
 * The LG Pay gift-card reader. Same story as MMEManager: the hardware path is
 * not here, and IS_ENABLED is the flag callers gate on.
 */
public class GiftcardManager {

    public static boolean IS_ENABLED = false;

    public GiftcardManager(Context context) {
    }

    public int initialize() {
        return -1;
    }

    public byte[] selectCard(byte[] aid) {
        return new byte[0];
    }

    public int startSignal() {
        return -1;
    }

    public int stopSignal() {
        return -1;
    }

    public int terminate() {
        return -1;
    }
}
