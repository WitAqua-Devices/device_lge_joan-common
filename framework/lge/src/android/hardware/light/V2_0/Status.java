/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.hardware.light.V2_0;

/** The light HAL's status enum. See LightState for why it is here. */
public final class Status {
    public static final int SUCCESS = 0;
    public static final int LIGHT_NOT_SUPPORTED = 1;
    public static final int BRIGHTNESS_NOT_SUPPORTED = 2;
    public static final int UNKNOWN = 3;

    private Status() {
    }

    public static String toString(int status) {
        switch (status) {
            case SUCCESS: return "SUCCESS";
            case LIGHT_NOT_SUPPORTED: return "LIGHT_NOT_SUPPORTED";
            case BRIGHTNESS_NOT_SUPPORTED: return "BRIGHTNESS_NOT_SUPPORTED";
            default: return "UNKNOWN";
        }
    }
}
