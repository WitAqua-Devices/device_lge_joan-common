/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.lge.hardware.touch.V1_0;

import android.os.IHwBinder;

/**
 * LG's touch HAL: firmware version and upgrade, self diagnosis, and the
 * debug switches the hidden menu's touch screens toggle.
 *
 * Not on this build - the touch controller is reached through the lineage
 * touch HAL instead - so this answers "no hardware" to everything.
 *
 * getService() hands back an instance rather than the null a real HIDL
 * lookup would give, because TouchHalInterface does not check: it stores
 * what it got and goes straight on to linkToDeath. On stock that never
 * mattered, since the service was always there.
 */
public interface ITouch {

    /** What every call reports, having no hardware to ask. */
    int NOT_SUPPORTED = -1;

    static ITouch getService() {
        return Absent.INSTANCE;
    }

    boolean linkToDeath(IHwBinder.DeathRecipient recipient, long cookie);

    interface getFirmwareVersionCallback { void onValues(int err, String version); }
    interface getCoverFirmwareVersionCallback { void onValues(int err, String version); }
    interface doSelfDiagnosisCallback { void onValues(int err, String result); }
    interface doCoverSelfDiagnosisCallback { void onValues(int err, String result); }
    interface doAftSelfDiagnosisCallback { void onValues(int err, String result); }
    interface getAutoTuneCallback { void onValues(int err, String value); }
    interface getDebugToolStateCallback { void onValues(int err, boolean enabled); }
    interface getDebugOptionStateCallback { void onValues(int err, int state); }
    interface getGripSuppressionStateCallback { void onValues(int err, boolean enabled); }

    void getFirmwareVersion(getFirmwareVersionCallback cb);
    void getCoverFirmwareVersion(getCoverFirmwareVersionCallback cb);
    void doSelfDiagnosis(doSelfDiagnosisCallback cb);
    void doCoverSelfDiagnosis(doCoverSelfDiagnosisCallback cb);
    void doAftSelfDiagnosis(doAftSelfDiagnosisCallback cb);
    void getAutoTune(getAutoTuneCallback cb);
    void getDebugToolState(getDebugToolStateCallback cb);
    void getDebugOptionState(getDebugOptionStateCallback cb);
    void getGripSuppressionState(getGripSuppressionStateCallback cb);

    int doFirmwareUpgrade();
    int doCoverFirmwareUpgrade();
    int setAutoTune(String value);
    int setDebugToolState(boolean enabled);
    int setDebugOptionState(int state);
    int setGripSuppressionState(boolean enabled);
    int setSensingState(boolean enabled);
    int setFontEnable(boolean enabled);

    /** The answer when there is no touch HAL to reach. */
    final class Absent implements ITouch {

        static final Absent INSTANCE = new Absent();

        private Absent() {
        }

        @Override public boolean linkToDeath(IHwBinder.DeathRecipient r, long cookie) { return false; }

        @Override public void getFirmwareVersion(getFirmwareVersionCallback cb) { cb.onValues(NOT_SUPPORTED, ""); }
        @Override public void getCoverFirmwareVersion(getCoverFirmwareVersionCallback cb) { cb.onValues(NOT_SUPPORTED, ""); }
        @Override public void doSelfDiagnosis(doSelfDiagnosisCallback cb) { cb.onValues(NOT_SUPPORTED, ""); }
        @Override public void doCoverSelfDiagnosis(doCoverSelfDiagnosisCallback cb) { cb.onValues(NOT_SUPPORTED, ""); }
        @Override public void doAftSelfDiagnosis(doAftSelfDiagnosisCallback cb) { cb.onValues(NOT_SUPPORTED, ""); }
        @Override public void getAutoTune(getAutoTuneCallback cb) { cb.onValues(NOT_SUPPORTED, ""); }
        @Override public void getDebugToolState(getDebugToolStateCallback cb) { cb.onValues(NOT_SUPPORTED, false); }
        @Override public void getDebugOptionState(getDebugOptionStateCallback cb) { cb.onValues(NOT_SUPPORTED, 0); }
        @Override public void getGripSuppressionState(getGripSuppressionStateCallback cb) { cb.onValues(NOT_SUPPORTED, false); }

        @Override public int doFirmwareUpgrade() { return NOT_SUPPORTED; }
        @Override public int doCoverFirmwareUpgrade() { return NOT_SUPPORTED; }
        @Override public int setAutoTune(String value) { return NOT_SUPPORTED; }
        @Override public int setDebugToolState(boolean enabled) { return NOT_SUPPORTED; }
        @Override public int setDebugOptionState(int state) { return NOT_SUPPORTED; }
        @Override public int setGripSuppressionState(boolean enabled) { return NOT_SUPPORTED; }
        @Override public int setSensingState(boolean enabled) { return NOT_SUPPORTED; }
        @Override public int setFontEnable(boolean enabled) { return NOT_SUPPORTED; }
    }
}
