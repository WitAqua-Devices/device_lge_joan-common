/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.android.lge.lgsvcitems;

import android.content.Context;

/**
 * The command side of the same stack as {@link LgSvcItems}, plus the two
 * questions the ITS screens ask before they use it. Reporting the tunnel as
 * down is the honest answer here and is also what those screens handle: they
 * say the service is unavailable rather than showing stale values.
 */
public class LgSvcCmd {

    /** What setCmdValue reports when there is nothing behind it. */
    public static final int FAILURE = -1;

    private static LgSvcCmd sInstance;

    public static synchronized LgSvcCmd getInstance(Context context) {
        if (sInstance == null) {
            sInstance = new LgSvcCmd();
        }
        return sInstance;
    }

    public String getCmdValue(int item) {
        return "";
    }

    public int setCmdValue(int item, String value) {
        return FAILURE;
    }

    /** Whether QCRIL's message tunnel is up. It is not. */
    public boolean getIQcrilMsgTunnelServiceStatus() {
        return false;
    }

    /** Operating-mode change (offline / online / factory test). */
    public boolean onSendOprtMode(int mode) {
        return false;
    }
}
