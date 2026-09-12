/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.security;

/**
 * The keystore facade apps used to reach directly.
 *
 * This one is not LG's - it was the platform's, and it went away when the
 * keystore moved behind KeyStore2. The hidden menu asks it one question,
 * whether an algorithm is backed by hardware, and joan answers yes: keymaster
 * runs in the TEE here as it did on stock.
 */
public class KeyStore {

    private static final KeyStore INSTANCE = new KeyStore();

    private KeyStore() {
    }

    public static KeyStore getInstance() {
        return INSTANCE;
    }

    public boolean isHardwareBacked(String algorithm) {
        return true;
    }
}
