/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.qualcomm.qcrilhook;

import android.content.Context;

/**
 * Qualcomm's OEM hook into RIL. The real one binds to the message tunnel
 * service that ships with QtiTelephonyService, which this build does not have,
 * so the one call the hidden menu makes reports failure rather than silently
 * claiming to have written the CDMA subscription source.
 *
 * Only the surface ECC and SmsCDG touch is here; the class exists so those
 * screens open at all.
 */
public class QcRilHook {

    public QcRilHook(Context context) {
    }

    public boolean qcRilSetCdmaSubSrcWithSpc(int subSource, String spc) {
        return false;
    }
}
