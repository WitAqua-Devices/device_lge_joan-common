/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.media;

/**
 * The thumbnail cache the media store used to keep next to its database.
 *
 * This one is not LG's - it was the platform's, and it went away with the
 * rewrite that moved thumbnails into MediaProvider. The one-seg app still
 * clears it when its recording list changes, which is the only thing it ever
 * asks for, and there is no longer a file to clear.
 */
public class MiniThumbFile {

    private MiniThumbFile() {
    }

    /** Drop the cached thumbnails. There are none to drop. */
    public static void reset() {
    }
}
