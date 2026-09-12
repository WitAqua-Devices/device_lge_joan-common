/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.hardware.light.V2_0;

/**
 * The light HAL's state struct, as the HIDL java bindings expose it.
 *
 * Those bindings are generated per device from android.hardware.light@2.0,
 * and nothing on this build asks for them, so they are not in the image. The
 * hidden menu's LED screens fill one of these in to hand to LG's ILightEx,
 * which is not here either - so this only has to hold the fields.
 */
public class LightState {
    public int color;
    public int flashMode;
    public int flashOnMs;
    public int flashOffMs;
    public int brightnessMode;
}
