/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.nfcaddon;

import android.content.Context;

/**
 * LG's side door into the NFC controller, used by the hidden menu's RF tests.
 * The stock path goes through their own NFC service; ours is AOSP's, which has
 * no equivalent, so the tests are told the commands did not take.
 */
public class NfcAdapterAddon {

    private static final NfcAdapterAddon sInstance = new NfcAdapterAddon();

    public static NfcAdapterAddon getNfcAdapterAddon(Context context) {
        return sInstance;
    }

    public boolean isNfcSystemEnabled() {
        return false;
    }

    public boolean sendNfcTestCommand(int command, byte[] payload) {
        return false;
    }

    public boolean setNfcPowerStatus(int status) {
        return false;
    }
}
