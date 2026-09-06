/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.mdm;

import android.content.ComponentName;

/**
 * LG's device-management policy store. Nothing is enrolled here, so the
 * restrictions it would answer for are all absent - which is to say everything
 * is allowed.
 */
public class LGMDMManager {

    private static final LGMDMManager sInstance = new LGMDMManager();

    public static LGMDMManager getInstance() {
        return sInstance;
    }

    public boolean getAllowHardwareFactoryreset(ComponentName who) {
        return true;
    }

    public boolean getAllowWipeData(ComponentName who) {
        return true;
    }
}
