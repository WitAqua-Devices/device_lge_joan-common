/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.ARUIServer;

import android.content.Context;
import android.graphics.Rect;
import android.view.KeyEvent;
import android.widget.FrameLayout;

/**
 * Hands out the AR UI overlay implementation. There is no overlay service on
 * this build, so a do-nothing instance is returned rather than null - the apps
 * call straight through the result of getInstance() without checking it.
 */
public class ARUIServer {

    private static final ARUIServerAPI sInstance = new ARUIServerAPI() {
        @Override
        public String getId() {
            return "";
        }

        @Override
        public boolean keyEvent(KeyEvent event) {
            /* Not handled, let the app deal with the key itself. */
            return false;
        }

        @Override
        public void start(Context context, ICallbacks callbacks, FrameLayout container,
                Rect bounds, boolean fullscreen) {
        }

        @Override
        public void stop() {
        }
    };

    public static ARUIServerAPI getInstance() {
        return sInstance;
    }
}
