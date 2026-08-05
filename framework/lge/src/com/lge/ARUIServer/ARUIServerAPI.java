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
 * LG's AR UI overlay service. Used by the data broadcast (Ginga) side of the TV
 * app to draw on top of the video.
 *
 * ICallbacks is deliberately empty: the app implements it, and an implementing
 * class having more methods than the interface declares is harmless, whereas
 * declaring a method the app does not implement would not be.
 */
public interface ARUIServerAPI {

    interface ICallbacks {
    }

    String getId();

    boolean keyEvent(KeyEvent event);

    void start(Context context, ICallbacks callbacks, FrameLayout container, Rect bounds,
            boolean fullscreen);

    void stop();
}
