/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.os;

/**
 * LG's wrapper for reading and writing vendor properties by code rather than by
 * name. The codes index a table that lives in LG's framework.jar, which is not
 * something this side has, so every accessor here behaves as if the code were
 * unknown: the setter drops the write and the getters hand back the default the
 * caller supplied.
 *
 * That is the answer the callers are written to cope with. HiddenMenu asks for
 * two of them at class-init time and only ever compares the result - code 0x70c
 * against "mtk" and code 0xaaf against "TRF_VZW" - so an empty string picks the
 * qualcomm, non-verizon branch, which is the one joan wants. Without the
 * getters at all the class initializer dies with NoSuchMethodError before any
 * of that, and the whole app is unreachable.
 */
public class PropertyUtils {

    private static final PropertyUtils sInstance = new PropertyUtils();

    public static PropertyUtils getInstance() {
        return sInstance;
    }

    public void set(int code, String value) {
    }

    public String get(int code, String defaultValue) {
        return defaultValue;
    }

    public boolean getBoolean(int code, boolean defaultValue) {
        return defaultValue;
    }

    public int getInt(int code, int defaultValue) {
        return defaultValue;
    }
}
