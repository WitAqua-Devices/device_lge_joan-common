/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.os;

/**
 * LG's wrapper for setting vendor properties from an app by code rather than by
 * name. Nothing on this side maps those codes, so the setter is a no-op.
 */
public class PropertyUtils {

    private static final PropertyUtils sInstance = new PropertyUtils();

    public static PropertyUtils getInstance() {
        return sInstance;
    }

    public void set(int code, String value) {
    }
}
