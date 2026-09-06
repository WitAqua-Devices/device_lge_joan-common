/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.sbp;

/**
 * LG's single-binary property table. Each PROP_ entry is a descriptor - the
 * property name and the value to fall back on - and get()/set() are the
 * accessors that take one. Nothing here maintains the table, so a read gives
 * back the descriptor's own fallback and a write is dropped.
 */
public class SingleBinaryProperties {

    public static final String[] PROP_RO_LGE_MODEL_NAME =
            { "ro.vendor.lge.model.name", "" };
    public static final String[] PROP_RO_LAOP = { "ro.lge.laop", "" };
    public static final String[] PROP_PERSIST_COTA_COUNTRY =
            { "persist.vendor.lge.cota.country", "" };
    public static final String[] PROP_PERSIST_COTA_OPERATOR =
            { "persist.vendor.lge.cota.operator", "" };
    public static final String[] PROP_PERSIST_SBP_SIM_OPERATOR =
            { "persist.vendor.lge.sbp.sim.operator", "" };
    public static final String[] PROP_PERSIST_LAOT_ENABLE =
            { "persist.vendor.lge.laot.enable", "" };
    public static final String[] PROP_PERSIST_LAOT_ICCID =
            { "persist.vendor.lge.laot.iccid", "" };
    public static final String[] PROP_PERSIST_SMARTCA_UNDO =
            { "persist.vendor.lge.smartca.undo", "" };

    public static Object get(Object[] property) {
        if (property != null && property.length > 1) {
            return property[1];
        }
        return "";
    }

    public static boolean set(Object[] property) {
        return false;
    }

    public static String getPersistLgPath() {
        return "/mnt/vendor/persist-lg";
    }
}
