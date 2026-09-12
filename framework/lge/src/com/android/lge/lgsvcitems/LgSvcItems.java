/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.android.lge.lgsvcitems;

import android.content.Context;

/**
 * LG's service items: numbered modem NV entries the SVC and carrier screens
 * read and write. Stock reaches them over qualcomm's QCRIL message tunnel,
 * which is a service inside LG's own telephony stack - none of that is here,
 * so every item reads back empty and every write reports failure.
 *
 * The alternative was letting the class stay missing, which took KDDI_ITS,
 * DCM_ITS, No_UIM_Mode and the MCFG screens down with NoClassDefFoundError
 * before they drew anything.
 */
public class LgSvcItems {

    /** What setCmdValue reports when there is nothing behind it. */
    public static final int FAILURE = -1;

    private static LgSvcItems sInstance;

    public static synchronized LgSvcItems getInstance(Context context) {
        if (sInstance == null) {
            sInstance = new LgSvcItems();
        }
        return sInstance;
    }

    public String getCmdValue(int item) {
        return "";
    }

    public int setCmdValue(int item, String value) {
        return FAILURE;
    }
}
