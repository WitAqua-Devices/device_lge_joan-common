/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.android.atservice.client;

import android.os.Bundle;

/**
 * The binder LGATCMDService publishes. Declared here rather than reimplemented
 * by hand because the generated marshalling has to match LG's byte for byte -
 * their service extends this Stub, and the apk carries no copy of its own.
 *
 * One method, taking "action" (the opcode) and "data" in a Bundle and
 * answering with "result" and "data". Same shape as the interface decoded out
 * of LG's framework.jar, which is what fixes the descriptor and the
 * transaction number.
 */
interface ILGATCMDService {
    Bundle request(in Bundle data);
}
