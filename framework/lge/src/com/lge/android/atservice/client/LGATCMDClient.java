/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.android.atservice.client;

import android.content.Context;

/**
 * Client for LG's AT command service.
 *
 * The service itself is not in this image - it lives behind an interface LG
 * added to their framework, with no APK or binary in the stock dump to carry
 * over - so this binds to nothing and every request fails. Callers check
 * {@link Response#result} against zero, so the screens that read radio
 * calibration, IMEI or serial numbers over AT commands come up and report no
 * data rather than taking the app down with a NoClassDefFoundError.
 */
public class LGATCMDClient {

    /** Anything other than 0 means the command did not run. */
    private static final int RESULT_FAILED = -1;

    public static class Response {
        public int result;
        public int status;
        public int length;
        public byte[] data;
    }

    public LGATCMDClient(Context context) {
    }

    public void bindService() {
    }

    public void unbindService() {
    }

    public boolean checkmBound() {
        return false;
    }

    public Response request(int command, byte[] arguments) {
        final Response response = new Response();
        response.result = RESULT_FAILED;
        response.status = RESULT_FAILED;
        response.length = 0;
        response.data = new byte[0];
        return response;
    }
}
