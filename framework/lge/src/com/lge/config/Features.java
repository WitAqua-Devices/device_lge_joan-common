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
 */
public class Features {
    public static final String LGE_FEATURE_QSLIDE = "false";
}
