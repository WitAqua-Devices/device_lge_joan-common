/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core;

/**
 * The pattern description LEDManager.startPattern() takes. Plain data - the
 * caller fills it in, and with no LED service on the other end nothing reads
 * it back.
 */
public class LGLedRecord {

    public int patternId;
    public String patternFilePath;
    public int whichLedPlay;
    public int priority;
    public int flags;
    public boolean infinite;

    public LGLedRecord() {
    }
}
