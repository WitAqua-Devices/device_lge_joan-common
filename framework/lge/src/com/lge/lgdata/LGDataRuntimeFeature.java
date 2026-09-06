/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.lgdata;

/**
 * One of LG's build-time feature flags, handed out as a singleton per flag.
 * None of them are on here, so every flag reports itself disabled and the code
 * behind it never runs.
 */
public class LGDataRuntimeFeature {

    public static final LGDataRuntimeFeature LGP_DATA_DISPLAY_IP_MPDN_KR_MTK =
            new LGDataRuntimeFeature();

    public boolean isEnabled() {
        return false;
    }

    public static void patchCodeId(String id) {
    }
}
