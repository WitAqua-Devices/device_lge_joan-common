/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.config;

/**
 * Build-time feature flags. LG generates this per model with the values baked
 * in as "true"/"false" strings, and callers compare against the string rather
 * than a boolean. QSlide is off here because the floating window it needs is
 * only a stub.
 *
 * The rest are the values joan's own lge_features.xml carries, and both
 * japanese SKUs agree with the global one on all three: a 6" POLED panel with
 * no notch, and no LAOP.
 */
public class Features {
    public static final String LGE_FEATURE_QSLIDE = "false";

    public static final String LGE_DISPLAY_TYPE = "POLED";
    public static final String LGE_DISPLAY_SHAPE = "NORMAL";
    public static final String LGE_FEATURE_LAOP = "false";
}
