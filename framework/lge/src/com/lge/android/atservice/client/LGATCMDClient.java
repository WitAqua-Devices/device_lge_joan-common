/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.android.atservice.client;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

/**
 * Client for LG's AT command service.
 *
 * The service itself is LGATCMDService, which this build ships alongside the
 * lgatcmd HAL and the atd daemon behind it. What is not shipped is LG's
 * framework.jar, where this class and the binder interface it uses lived, so
 * both are here instead: the wire format comes from the one decoded out of
 * their framework, which is what lets their service answer this client.
 *
 * When the service is not installed - or has not bound yet - requests fall
 * back to a synthetic "Unavailable" answer rather than null. The callers are
 * not careful with null: VersionInfo reads response.data with no check a few
 * lines after checking the response itself, and MID Info hands the bytes
 * straight to a StringTokenizer. Answering with a real Response keeps those
 * screens up and honest about having no value.
 */
public class LGATCMDClient {

    private static final String TAG = "LGATCMDClient";

    /** The largest command LG's service accepts. */
    public static final int MAX_LG_COMMAND_SIZE = 0x800;

    /** Their service answers with this much room for the payload. */
    private static final int RESPONSE_BUFFER_SIZE = 0x80c;

    /** Anything other than 0 means the command did not run. */
    private static final int RESULT_FAILED = -1;

    /** What the screens show in place of a value nothing can answer for. */
    private static final byte[] UNAVAILABLE = "Unavailable".getBytes();

    public static class Response {
        public int result;
        public int status;
        public int length;
        public byte[] data;

        Response() {
            data = new byte[RESPONSE_BUFFER_SIZE];
        }

        /**
         * Their service answers with one flat buffer: result, length and
         * status as little endian ints, then the payload. data is left the
         * full size and zeroed past length, so it reads back as a C string -
         * which is how the callers treat it, scanning for the terminator
         * rather than trusting length.
         */
        Response(byte[] raw) {
            this();
            result = readInt(raw, 0);
            length = readInt(raw, 4);
            status = readInt(raw, 8);
            if (length < 0 || length > data.length || 12 + length > raw.length) {
                Log.e(TAG, "Response: bad length " + length + " for " + raw.length + " bytes");
                length = Math.max(0, Math.min(data.length, raw.length - 12));
            }
            System.arraycopy(raw, 12, data, 0, length);
        }

        private static int readInt(byte[] b, int off) {
            return (b[off] & 0xff)
                    | ((b[off + 1] & 0xff) << 8)
                    | ((b[off + 2] & 0xff) << 16)
                    | ((b[off + 3] & 0xff) << 24);
        }
    }

    private final Object mLock = new Object();
    private final Context mContext;

    private ILGATCMDService mService;
    private boolean mBound;
    private boolean mSuccess;

    private final ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder binder) {
            synchronized (mLock) {
                mService = ILGATCMDService.Stub.asInterface(binder);
                mBound = true;
            }
            Log.d(TAG, "Service connected");
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            synchronized (mLock) {
                mService = null;
                mBound = false;
            }
        }
    };

    public LGATCMDClient(Context context) {
        mContext = context;
    }

    public void bindService() {
        synchronized (mLock) {
            if (mBound) {
                return;
            }
            final Intent intent = new Intent().setComponent(new ComponentName(
                    "com.lge.android.atservice",
                    "com.lge.android.atservice.LGATCMDService"));
            mSuccess = mContext.bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
            if (mSuccess) {
                Log.d(TAG, "Bind service successfully");
            } else {
                Log.e(TAG, "Bind service fail!!");
            }
        }
    }

    public void unbindService() {
        synchronized (mLock) {
            if (!mSuccess) {
                return;
            }
            mContext.unbindService(mConnection);
            mService = null;
            mBound = false;
            mSuccess = false;
        }
    }

    public boolean checkBindSuccess() {
        synchronized (mLock) {
            return mSuccess;
        }
    }

    /**
     * Whether a request can be made. The callers poll this after bindService()
     * and only then start asking - MID Info gives it five seconds and then
     * gives up with an empty screen.
     *
     * Bound for real, or never going to be: bindService() answers straight
     * away but onServiceConnected lands a few milliseconds later, and a caller
     * that starts asking in between gets the fallback for every value. So this
     * says yes once the connection is up, and also when the bind itself failed
     * - there is nothing to wait for if the service is not installed at all.
     */
    public boolean checkmBound() {
        synchronized (mLock) {
            return mBound || !mSuccess;
        }
    }

    public Response request(int command, byte[] arguments) {
        synchronized (mLock) {
            if (mService != null) {
                try {
                    final Bundle in = new Bundle();
                    in.putInt("action", command);
                    in.putByteArray("data", arguments);
                    final Bundle out = mService.request(in);
                    if (out != null && out.getInt("result") == 0) {
                        final byte[] raw = out.getByteArray("data");
                        if (raw != null && raw.length >= 12) {
                            return new Response(raw);
                        }
                    }
                    Log.e(TAG, "request: command " + Integer.toHexString(command) + " failed");
                } catch (RemoteException e) {
                    Log.e(TAG, "request: service died", e);
                }
            }
        }
        return unavailable();
    }

    public String getFactoryVersion() {
        final Response response = request(0xfb1, "".getBytes());
        return new String(response.data, 0, response.length);
    }

    private static Response unavailable() {
        final Response response = new Response();
        response.result = RESULT_FAILED;
        response.status = RESULT_FAILED;
        response.length = UNAVAILABLE.length;
        System.arraycopy(UNAVAILABLE, 0, response.data, 0, UNAVAILABLE.length);
        return response;
    }
}
