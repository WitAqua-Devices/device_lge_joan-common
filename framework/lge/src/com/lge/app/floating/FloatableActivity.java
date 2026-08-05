/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.app.floating;

import android.app.Activity;

/**
 * Base class LG uses for activities that can be shown in a floating window.
 *
 * The stock apps only ever reach it through the normal Activity lifecycle -
 * onCreate, onStart, onResume, onPause, onStop, onDestroy and
 * onWindowFocusChanged - so an empty subclass of Activity is enough to make
 * their invoke-super calls resolve. Nothing floats, the activity just behaves
 * like any other full screen one.
 */
public class FloatableActivity extends Activity {
}
