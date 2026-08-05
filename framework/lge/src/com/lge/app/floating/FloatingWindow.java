/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.app.floating;

import android.content.res.Resources;
import android.view.View;

/**
 * The window a FloatableActivity would be hosted in. Since nothing floats here,
 * every accessor returns nothing and the mutators do nothing; callers already
 * have to cope with a null frame view because the window does not exist until
 * the activity is actually floated.
 */
public class FloatingWindow {

    /**
     * What the host activity implements to follow the frame around. Nothing
     * calls back into it here, but the app implements it at class-load time, so
     * the whole shape has to exist. The methods that return a boolean are the
     * ones LG lets the app veto - true means "I handled it".
     */
    public interface OnUpdateListener {
        boolean onCloseRequested(FloatingWindow window);

        void onClosing(FloatingWindow window);

        boolean onEnteringLowProfileMode(FloatingWindow window, boolean animate);

        boolean onExitingLowProfileMode(FloatingWindow window, boolean animate);

        void onMoveCanceled(FloatingWindow window);

        void onMoveFinished(FloatingWindow window, int x, int y);

        void onMoveStarted(FloatingWindow window);

        void onMoveToTop(FloatingWindow window);

        void onMoving(FloatingWindow window, int x, int y);

        void onResizeCanceled(FloatingWindow window);

        void onResizeFinished(FloatingWindow window, int width, int height);

        void onResizeStarted(FloatingWindow window);

        void onResizing(FloatingWindow window, int width, int height);

        boolean onSwitchFullRequested(FloatingWindow window);

        void onSwitchingFull(FloatingWindow window);

        void onSwitchingMinimized(FloatingWindow window, boolean minimized);

        void onTitleViewTouch(FloatingWindow window, android.view.MotionEvent event);
    }

    /** Geometry LG hands out for the floating frame. Fields are public in the
     *  original, and the apps read and write them directly. */
    public static class LayoutParams {
        public int x;
        public int y;
        public int width;
        public int height;
        public float minWidthWeight;
        public float minHeightWeight;
        public float maxWidthWeight;
        public float maxHeightWeight;
        public int resizeOption;
        public boolean hideTitle;
        public boolean dontUseIme;
        public boolean useDoubleTapMinimize;
    }

    public void close(boolean animate) {
    }

    public View findViewWithTag(Object tag) {
        return null;
    }

    public View getFloatingFrameView() {
        return null;
    }

    public Resources getFloatingWindowResources() {
        return null;
    }

    public LayoutParams getLayoutParams() {
        return new LayoutParams();
    }

    public void updateLayoutParams(LayoutParams params) {
    }
}
