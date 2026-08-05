/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "mtvmedia_shim"

#include <log/log.h>
#include <media/AudioTrack.h>

#include <new>

/*
 * libmtv_servicejp.lge.so builds its AudioTrack with the android 9 constructor.
 * Every other AudioTrack method it calls is still exported by libaudioclient
 * and resolves there - only this one constructor is gone, because two of its
 * parameters were replaced:
 *
 *   callback_t cbf + void* user  ->  const wp<IAudioTrackCallback>&
 *   uid_t uid + pid_t pid        ->  const AttributionSourceState&
 *
 * So the object has to be a real, current AudioTrack: the caller allocated the
 * storage and then calls into libaudioclient with it. This constructs one in
 * place and adapts those two parameters. The size of the caller's allocation is
 * android 9's sizeof(AudioTrack); the current one is logged on first use so a
 * mismatch is visible rather than silent.
 */

namespace android {

namespace {

// Bridges the old function-pointer callback onto the interface AudioTrack now
// takes. The event numbers are the android 9 AudioTrack::event_type values,
// which is what the stock library's handler switches on.
class LegacyCallback : public AudioTrack::IAudioTrackCallback {
public:
    using callback_t = void (*)(int event, void* user, void* info);

    LegacyCallback(callback_t cbf, void* user) : mCbf(cbf), mUser(user) {}

protected:
    enum {
        EVENT_MORE_DATA = 0,
        EVENT_UNDERRUN = 1,
        EVENT_LOOP_END = 2,
        EVENT_MARKER = 3,
        EVENT_NEW_POS = 4,
        EVENT_BUFFER_END = 5,
        EVENT_NEW_IAUDIOTRACK = 6,
        EVENT_STREAM_END = 7,
    };

    size_t onMoreData(const AudioTrack::Buffer& buffer) override {
        // The old callback wrote into the Buffer it was handed and reported
        // back through Buffer::size, so it has to see a writable copy.
        AudioTrack::Buffer copy = buffer;
        mCbf(EVENT_MORE_DATA, mUser, &copy);
        return copy.size();
    }

    void onUnderrun() override { mCbf(EVENT_UNDERRUN, mUser, nullptr); }

    void onLoopEnd(int32_t loopsRemaining) override {
        mCbf(EVENT_LOOP_END, mUser, &loopsRemaining);
    }

    void onMarker(uint32_t markerPosition) override {
        mCbf(EVENT_MARKER, mUser, &markerPosition);
    }

    void onNewPos(uint32_t newPos) override { mCbf(EVENT_NEW_POS, mUser, &newPos); }

    void onBufferEnd() override { mCbf(EVENT_BUFFER_END, mUser, nullptr); }

    void onNewIAudioTrack() override { mCbf(EVENT_NEW_IAUDIOTRACK, mUser, nullptr); }

    void onStreamEnd() override { mCbf(EVENT_STREAM_END, mUser, nullptr); }

private:
    const callback_t mCbf;
    void* const mUser;
};

// The adapters have to outlive the track, which only holds a weak pointer.
// There is one tuner, so the handful this ever creates is not worth reclaiming.
sp<LegacyCallback> keepAlive(const sp<LegacyCallback>& cb) {
    static std::vector<sp<LegacyCallback>>* held = new std::vector<sp<LegacyCallback>>();
    held->push_back(cb);
    return cb;
}

}  // namespace

extern "C" void _ZN7android10AudioTrackC1E19audio_stream_type_tj14audio_format_tjj20audio_output_flags_tPFviPvS4_ES4_i15audio_session_tNS0_13transfer_typeEPK20audio_offload_info_tjiPK18audio_attributes_tbfi(
        void* self, audio_stream_type_t streamType, uint32_t sampleRate, audio_format_t format,
        uint32_t channelMask, uint32_t frameCount, audio_output_flags_t flags,
        void (*cbf)(int, void*, void*), void* user, int32_t notificationFrames,
        audio_session_t sessionId, AudioTrack::transfer_type transferType,
        const audio_offload_info_t* offloadInfo, uint32_t uid, int32_t pid,
        const audio_attributes_t* pAttributes, bool doNotReconnect, float maxRequiredSpeed,
        int32_t selectedDeviceId) {
    ALOGI("legacy AudioTrack ctor: sizeof(AudioTrack)=%zu stream=%d rate=%u fmt=%#x",
          sizeof(AudioTrack), streamType, sampleRate, format);

    wp<AudioTrack::IAudioTrackCallback> callback;
    if (cbf != nullptr) {
        callback = keepAlive(sp<LegacyCallback>::make(cbf, user));
    }

    AttributionSourceState attributionSource;
    attributionSource.uid = static_cast<int32_t>(uid);
    attributionSource.pid = pid;
    attributionSource.token = sp<BBinder>::make();

    new (self) AudioTrack(streamType, sampleRate, format,
                          static_cast<audio_channel_mask_t>(channelMask), frameCount, flags,
                          callback, notificationFrames, sessionId, transferType, offloadInfo,
                          attributionSource, pAttributes, doNotReconnect, maxRequiredSpeed,
                          static_cast<audio_port_handle_t>(selectedDeviceId));
}

}  // namespace android
