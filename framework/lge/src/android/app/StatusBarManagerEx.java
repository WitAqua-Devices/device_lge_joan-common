/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.app;

import android.content.Context;

/**
 * LG's extension of StatusBarManager.
 *
 * Nothing ever holds one of these here. The callers reach it by casting what
 * getSystemService("statusbar") returned, which on this build is a plain
 * StatusBarManager, so the cast throws - inside the try/catch the one-seg app
 * already wraps it in. The class still has to exist for that cast to be
 * compiled and resolved at all, and the status bar simply stays where it is.
 */
public class StatusBarManagerEx extends StatusBarManager {

    public StatusBarManagerEx(Context context) {
        super(context);
    }

    /** LG's two-argument form: a mask of what to hide, and which bar. */
    public void disable(int what, int which) {
    }
}
