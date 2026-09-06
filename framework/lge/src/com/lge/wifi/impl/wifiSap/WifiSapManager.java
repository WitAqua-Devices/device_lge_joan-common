/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.wifi.impl.wifiSap;

import java.util.Collections;
import java.util.List;

/**
 * LG's softap station list. The platform reports connected stations through
 * SoftApCallback instead, so this answers with an empty list - again not null,
 * because the caller iterates it.
 */
public class WifiSapManager {

    private static final WifiSapManager sInstance = new WifiSapManager();

    public static WifiSapManager getInstance() {
        return sInstance;
    }

    public List<?> getAllStaDetails() {
        return Collections.emptyList();
    }
}
