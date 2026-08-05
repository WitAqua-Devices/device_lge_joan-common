/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.app.permission;

import android.app.Activity;

/**
 * Supplies the strings LG's runtime-permission guide dialog shows when the user
 * has permanently denied a permission. Apps subclass DefaultUiProvider and
 * override only what they need, so every method is defaulted here.
 */
public interface GuideUiProvider {

    default String getTitle(Activity activity, String[] permissions) {
        return null;
    }

    default String getMessage(Activity activity, String[] permissions) {
        return null;
    }
}
