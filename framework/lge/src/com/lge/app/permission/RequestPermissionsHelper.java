/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.app.permission;

import android.app.Activity;

/**
 * LG's helper for onRequestPermissionsResult: it puts up a guide dialog when a
 * permission was permanently denied and reports whether it handled the result.
 * AOSP has no such dialog, so nothing is handled and the caller falls back to
 * its own path.
 */
public class RequestPermissionsHelper {

    public static boolean handlePermissionRequestResult(Activity activity, String[] permissions,
            boolean granted, GuideUiProvider provider) {
        return false;
    }
}
