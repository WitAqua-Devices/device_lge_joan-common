/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.media;

import android.content.Context;

/**
 * The audio API LG adds by patching framework.jar, in the platform's own
 * package rather than com.lge.
 *
 * The one-seg app builds one of these in TdmbAudioManager's constructor, so
 * the class not being here took the whole app down before it drew anything:
 *
 *   NoClassDefFoundError: Failed resolution of: Landroid/media/AudioManagerEx;
 *     at com.lge.oneseg.util.TdmbAudioManager.<init>(TdmbAudioManager.java:25)
 *
 * Stock derives it from AudioManager and adds to it, so this does too - the
 * app keeps the field typed as AudioManagerEx and would not survive being
 * handed something that is not an AudioManager.
 */
public class AudioManagerEx extends AudioManager {

    private static final String TAG = "AudioManagerEx";

    private boolean mSpeakerOnForMedia;

    public AudioManagerEx(Context context) {
        super(context);
    }

    /**
     * Whether the wired antenna adapter is plugged in.
     *
     * On the japanese handsets the one-seg antenna is the earphone cable, so
     * the honest answer is whether anything is in the jack - which the
     * platform can say for itself. Stock asked its own audio service and
     * returned false when that was not there; answering from the device list
     * is closer to the question the caller is actually asking, which it logs
     * as "isWiredAntennaGender".
     */
    public boolean getAuxGenderState() {
        for (AudioDeviceInfo device : getDevices(GET_DEVICES_OUTPUTS)) {
            switch (device.getType()) {
                case AudioDeviceInfo.TYPE_WIRED_HEADSET:
                case AudioDeviceInfo.TYPE_WIRED_HEADPHONES:
                    return true;
                default:
                    break;
            }
        }
        return false;
    }

    /**
     * LG's own "play media through the speaker anyway" switch, which is how
     * one-seg gets sound out of the speaker while the antenna occupies the
     * headphone jack.
     *
     * Nothing in AOSP does that - the routing decision belongs to the policy
     * manager and there is no public way to override it for media - so this
     * only remembers what it was told. The app's own toggle stays consistent
     * with itself; the sound still follows the jack.
     */
    public boolean isSpeakerOnForMedia() {
        return mSpeakerOnForMedia;
    }

    public void setSpeakerOnForMedia(boolean on) {
        mSpeakerOnForMedia = on;
    }
}
