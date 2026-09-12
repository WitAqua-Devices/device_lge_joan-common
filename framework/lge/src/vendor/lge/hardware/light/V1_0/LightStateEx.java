/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.lge.hardware.light.V1_0;

import android.hardware.light.V2_0.LightState;

/** LG's light state: the platform's, with their own two fields beside it. */
public class LightStateEx {
    public LightState base = new LightState();
    public int flashMode;
    public int brightnessMode;
}
