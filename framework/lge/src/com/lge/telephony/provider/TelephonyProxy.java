/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.telephony.provider;

import android.net.Uri;

/**
 * The extra tables LG hangs off the telephony provider.
 *
 * The authority is the platform's own, so these URIs are the stock ones
 * verbatim; what is missing is the provider side, since AOSP's
 * TelephonyProvider knows nothing about mapcon, ike or the backup apn tables.
 * The class still has to exist: the hidden menu's mapcon and IKE editors name
 * it in a static initializer, so without it they cannot even be loaded, and
 * the failure is a class that is not there rather than a query that finds
 * nothing.
 */
public final class TelephonyProxy {

    private TelephonyProxy() {
    }

    public static final class Carriers {
        public static final Uri CONTENT_ADMIN_URI =
                Uri.parse("content://telephony/carriers/admin_status");
        public static final Uri CONTENT_BACKUP_URI =
                Uri.parse("content://telephony/carriers/backup_apn");

        private Carriers() {
        }
    }

    public static final class Ike {
        public static final Uri CONTENT_URI = Uri.parse("content://telephony/ike");

        private Ike() {
        }
    }

    public static final class Mapcon {
        public static final Uri CONTENT_URI = Uri.parse("content://telephony/mapcon");

        private Mapcon() {
        }
    }
}
