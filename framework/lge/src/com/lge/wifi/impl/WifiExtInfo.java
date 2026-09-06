/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.wifi.impl;

/**
 * The extra connection detail LG's supplicant reports alongside WifiInfo. The
 * platform supplicant does not carry it, so every field reads back empty rather
 * than null - the hidden menu prints these straight into a TextView.
 */
public class WifiExtInfo {

    public String getKeyMgmt() {
        return "";
    }

    public String getCipher() {
        return "";
    }

    public String getEAPMETHOD() {
        return "";
    }

    public String getDevMode() {
        return "";
    }
}
