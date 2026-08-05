/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core;

import android.content.Context;

/**
 * Entry point to LG's own system services. None of them exist here, so the
 * lookup always comes up empty - callers treat that the same way they treat a
 * service that failed to start.
 */
public class LGContext {

    public LGContext(Context context) {
    }

    public Object getLGSystemService(String name) {
        return null;
    }
}
