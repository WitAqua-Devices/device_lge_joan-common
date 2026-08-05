/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.os;

import android.os.SystemProperties;

/**
 * The build identity LG's own apps read instead of {@link android.os.Build}.
 *
 * These are the fields the hidden menu touches, and the properties behind them
 * are the ones the vendor image still sets, so a device that boots as an L-01K
 * gets DCM/JP here the same way stock did.
 */
public class Build {

    /** Which carrier and market the image was built for. */
    public static class CA_TARGET {
        public static final String OPERATOR =
                SystemProperties.get("ro.vendor.lge.build.target_operator", "GLOBAL");
        public static final String COUNTRY =
                SystemProperties.get("ro.vendor.lge.build.target_country", "COM");
        public static final String REGION =
                SystemProperties.get("ro.vendor.lge.build.target_region", "");
    }

    public static class VERSION {
        /**
         * Whether the device shipped on an older platform release and was
         * updated into this one. Nothing here carries that history, and a
         * clean build is not an upgrade, so it is false.
         */
        public static final boolean IS_OS_UPGRADED = false;

        /** The platform release the device shipped with. See above. */
        public static final String ORIGINAL_RELEASE = android.os.Build.VERSION.RELEASE;
    }

    /** LG's own UX version, which they number separately from android's. */
    public static class LGUI_VERSION {
        public static final int RELEASE = majorOf(
                SystemProperties.get("ro.vendor.lge.lguiversion", ""));

        private static int majorOf(String version) {
            final int dot = version.indexOf('.');
            try {
                return Integer.parseInt(dot < 0 ? version : version.substring(0, dot));
            } catch (NumberFormatException e) {
                return 0;
            }
        }
    }
}
