/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.wifi.impl;

/**
 * Hands out the WifiExtInfo above. Returns an empty one rather than null so the
 * callers, which do not check, get blank fields instead of an NPE.
 */
public class WifiExtManager {

    private static final WifiExtManager sInstance = new WifiExtManager();
    private static final WifiExtInfo sInfo = new WifiExtInfo();

    public static WifiExtManager getInstance() {
        return sInstance;
    }

    public WifiExtInfo getConnectionExtInfo() {
        return sInfo;
    }
}
