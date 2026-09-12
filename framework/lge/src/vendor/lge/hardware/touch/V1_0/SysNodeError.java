/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.lge.hardware.touch.V1_0;

/** What LG's touch HAL returns when a sysfs node does not answer. */
public final class SysNodeError {
    public static final int NONE = 0;
    public static final int OPEN = 1;
    public static final int READ = 2;
    public static final int WRITE = 3;

    private SysNodeError() {
    }

    public static String toString(int error) {
        switch (error) {
            case NONE: return "NONE";
            case OPEN: return "OPEN";
            case READ: return "READ";
            case WRITE: return "WRITE";
            default: return "0x" + Integer.toHexString(error);
        }
    }
}
