/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core.halinterface;

/**
 * Reached through LGContext.getLGSystemService(). LG throttles charging while
 * the tuner runs to keep the phone cool; without their charger HAL there is
 * nothing to ask, so the request is dropped.
 */
public class ChargerHalManager {

    public void setRestrictChargingMode(int mode) {
    }
}
