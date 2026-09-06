/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.hardware.display;

import android.os.IBinder;
import android.os.IInterface;
import android.os.RemoteException;

/**
 * The extra binder interface LG's display service publishes alongside the
 * platform's own, under the same "display" name.
 *
 * BrightnessUtility asks ServiceManager for "display" and casts the result to
 * this, then skips the whole brightness path when the cast comes back null.
 * That is the branch this is here to reach: asInterface only answers for a
 * binder that actually claims to be one, and the platform's display service
 * does not, so it returns null and one-seg leaves the screen brightness alone.
 */
public interface IDisplayManagerEx extends IInterface {

    String DESCRIPTOR = "android.hardware.display.IDisplayManagerEx";

    void setTemporaryBrightness(int displayId, int brightness) throws RemoteException;

    abstract class Stub extends android.os.Binder implements IDisplayManagerEx {

        public Stub() {
            attachInterface(this, DESCRIPTOR);
        }

        public static IDisplayManagerEx asInterface(IBinder obj) {
            if (obj == null) {
                return null;
            }
            IInterface local = obj.queryLocalInterface(DESCRIPTOR);
            return local instanceof IDisplayManagerEx ? (IDisplayManagerEx) local : null;
        }

        @Override
        public IBinder asBinder() {
            return this;
        }
    }
}
