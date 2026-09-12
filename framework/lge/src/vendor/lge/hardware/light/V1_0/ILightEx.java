/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.lge.hardware.light.V1_0;

import android.hardware.light.V2_0.Status;
import android.os.IHwBinder;

/**
 * LG's extension of the light HAL - the notification LED, the cover LED and
 * the patterns they can play.
 *
 * There is no such HAL here, and joan drives its LEDs through the ordinary
 * lights path, so this answers "not supported" to everything.
 *
 * getService() hands back an instance rather than the null a real HIDL lookup
 * would give, because LightExHalInterface does not check: it stores what it
 * got and goes straight on to linkToDeath. On stock that never mattered,
 * since the service was always there.
 */
public interface ILightEx {

    static ILightEx getService() {
        return Absent.INSTANCE;
    }

    boolean linkToDeath(IHwBinder.DeathRecipient recipient, long cookie);

    int setLightEx(int type, LightStateEx state);

    int setPattern(int pattern);
    int setRedBrightness(int brightness);
    int setGreenBrightness(int brightness);
    int setBlueBrightness(int brightness);

    int setCoverPattern(int pattern);
    int setCoverRedBrightness(int brightness);
    int setCoverGreenBrightness(int brightness);
    int setCoverBlueBrightness(int brightness);
    int setCoverLEDInit(int mode);

    /** The answer when there is no light HAL to reach. */
    final class Absent implements ILightEx {

        static final Absent INSTANCE = new Absent();

        private Absent() {
        }

        @Override public boolean linkToDeath(IHwBinder.DeathRecipient r, long cookie) { return false; }

        @Override public int setLightEx(int type, LightStateEx state) { return Status.LIGHT_NOT_SUPPORTED; }

        @Override public int setPattern(int pattern) { return Status.LIGHT_NOT_SUPPORTED; }
        @Override public int setRedBrightness(int brightness) { return Status.LIGHT_NOT_SUPPORTED; }
        @Override public int setGreenBrightness(int brightness) { return Status.LIGHT_NOT_SUPPORTED; }
        @Override public int setBlueBrightness(int brightness) { return Status.LIGHT_NOT_SUPPORTED; }

        @Override public int setCoverPattern(int pattern) { return Status.LIGHT_NOT_SUPPORTED; }
        @Override public int setCoverRedBrightness(int brightness) { return Status.LIGHT_NOT_SUPPORTED; }
        @Override public int setCoverGreenBrightness(int brightness) { return Status.LIGHT_NOT_SUPPORTED; }
        @Override public int setCoverBlueBrightness(int brightness) { return Status.LIGHT_NOT_SUPPORTED; }
        @Override public int setCoverLEDInit(int mode) { return Status.LIGHT_NOT_SUPPORTED; }
    }
}
