/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.lge.hardware.laop.V1_0;

/**
 * LG's LAOP HAL, which writes the operator provisioning files under
 * /vendor/lgdata.
 *
 * The hidden menu's FlexGPIOReceiver reaches for it from BOOT_COMPLETED, and
 * the class not being here took the whole app down every time its process
 * started - the broadcast is redelivered after the crash, so it never settled:
 *
 *   NoClassDefFoundError: Failed resolution of:
 *     Lvendor/lge/hardware/laop/V1_0/ILgeLaop;
 *       at com.lge.hiddenmenu.FlexGPIO.FlexGPIOReceiver.onReceive
 *
 * There is no LAOP HAL here and LGE_FEATURE_LAOP is false, so this answers
 * that no provisioning file exists and drops what it is asked to write.
 *
 * getService() hands back an instance rather than the null a real HIDL lookup
 * would give. The receiver does check, and would have been happy either way -
 * a missing file leads to the same branch as a missing service - but the
 * FlexGPIO screen behind it does not check before calling isFileExist.
 */
public interface ILgeLaop {

    static ILgeLaop getService() {
        return Absent.INSTANCE;
    }

    boolean isFileExist(String path);

    void writeVendorFile(String path, String name, String value);

    void deleteVendorFile(String path);

    /** The answer when there is no LAOP HAL to reach. */
    final class Absent implements ILgeLaop {

        static final Absent INSTANCE = new Absent();

        private Absent() {
        }

        @Override public boolean isFileExist(String path) { return false; }

        @Override public void writeVendorFile(String path, String name, String value) { }

        @Override public void deleteVendorFile(String path) { }
    }
}
