/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.database.sqlite;

import android.database.sqlite.SQLiteException;

/**
 * LG's helper around SQLite failures. The stock apps only ask it to tell a
 * genuine out-of-space failure apart from any other SQLiteException, so that
 * they can show a "storage full" message instead of crashing.
 */
public class SqliteWrapper {

    public static boolean isLowMemory(SQLiteException e) {
        if (e == null) {
            return false;
        }

        final String message = e.getMessage();
        return message != null && message.contains("database or disk is full");
    }
}
