/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package org.lineageos.hiddenmenu;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

/**
 * Opens LG's hidden menu, which has no launcher entry of its own.
 *
 * The activity is theme-less and finishes immediately, so all the user ever
 * sees is the hidden menu coming up.
 */
public class HiddenMenuShortcutActivity extends Activity {

    private static final String TAG = "HiddenMenuShortcut";

    private static final ComponentName TARGET = new ComponentName(
            "com.lge.hiddenmenu", "com.lge.hiddenmenu.HiddenMenu");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final Intent intent = new Intent(Intent.ACTION_MAIN)
                .setComponent(TARGET)
                .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        try {
            startActivity(intent);
        } catch (ActivityNotFoundException | SecurityException e) {
            // Not installed on this build, or com.lge.permission.LGHIDDEN went
            // to a different signer than the one that signed this.
            Log.e(TAG, "Cannot open " + TARGET.flattenToShortString(), e);
            Toast.makeText(this, R.string.hidden_menu_unavailable, Toast.LENGTH_LONG).show();
        }
        finish();
    }
}
