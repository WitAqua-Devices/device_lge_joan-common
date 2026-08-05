/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 * An android 9 shaped NuPlayerDriver, and the AudioSink that goes with it.
 *
 * MobitPlayerService drives playback through a NuPlayerDriver it builds itself,
 * with the vtable slot numbers frozen at android 9. Two things moved since, and
 * both are silent - the calls land on real functions, just the wrong ones:
 *
 *   MediaPlayerBase   setDataSource(const String8&) was added for RTP and sits
 *                     at slot 13, so everything from setVideoSurfaceTexture on
 *                     is one slot late. What the blob asks for and what it gets:
 *                     prepareAsync->prepare, start->prepareAsync, stop->start,
 *                     pause->stop, seekTo->getSyncSettings, reset->getDuration,
 *                     setLooping->notifyAt, invoke->setNextPlayer, and
 *                     setAudioSink->releaseDrm - which is why the blob's audio
 *                     output never reached the player at all.
 *
 *   AudioSink         setPlayerIId() was added after open() and setHasVideo()
 *                     was removed, so the slot count is unchanged at 41 but
 *                     everything between them is one slot late. open() kept its
 *                     slot and changed its signature: the callback now takes
 *                     sp<AudioSink>/wp<RefBase> where it took raw pointers.
 *
 * Neither is patchable in place - the driver's slots are read from a vtable the
 * framework owns, and the sink's vtable is baked into the blob - so this bridges
 * them instead:
 *
 *   blob --(android 9 slots)--> LegacyNuDriver --> NuPlayerDriver  (framework)
 *   blob's AudioOutput <--(android 9 slots)-- AudioSinkBridge <--  (framework)
 *
 * LegacyNuDriver is what the blob thinks it built: extract-files.py renames the
 * one symbol it looks up by name, _ZN7android14NuPlayerDriverC1Ei, and every
 * other call goes through the vtable this file emits. Same trick as
 * memoryheap_shim.cpp, and it needs the same two things to hold - the virtual
 * order has to be android 9's exactly, and sizeof has to fit the allocation the
 * blob makes (0x80). Both are checked below.
 *
 * The orders here are not read off android 9 source; they are what the stock
 * libmediaplayerservice.so in the phone dump actually emitted, decoded with
 * tools/abicheck.py, cross-checked against the blob's own AudioOutput vtable.
 */

#define LOG_TAG "mtv_nuplayer_shim"

#include <netinet/in.h>

#include <media/AVSyncSettings.h>
#include <media/AudioResamplerPublic.h>
#include <media/BufferingSettings.h>
#include <media/DataSource.h>
#include <media/IMediaHTTPService.h>
#include <media/IStreamSource.h>
#include <media/MediaPlayerInterface.h>
#include <nuplayer/NuPlayerDriver.h>
#include <utils/Log.h>

namespace android {

// ---------------------------------------------------------------------------
// The android 9 MediaPlayerBase::AudioSink.
//
// Never instantiated: the blob's MobitPlayerService::AudioOutput is one of
// these, and this declaration exists so its slots can be called by the numbers
// it was built with. RefBase first, so the destructor pair and RefBase's four
// hooks take slots 0-5 the way they do there.

class Legacy9AudioSink : public RefBase {
public:
    typedef MediaPlayerBase::AudioSink::cb_event_t cb_event_t;

    // Raw pointers, not sp<>/wp<>, which is the whole difference.
    typedef size_t (*AudioCallback)(void* audioSink, void* buffer, size_t size, void* cookie,
                                    cb_event_t event);

    virtual bool ready() const = 0;                                             //  6
    virtual ssize_t bufferSize() const = 0;                                     //  7
    virtual ssize_t frameCount() const = 0;                                     //  8
    virtual ssize_t channelCount() const = 0;                                   //  9
    virtual ssize_t frameSize() const = 0;                                      // 10
    virtual uint32_t latency() const = 0;                                       // 11
    virtual float msecsPerFrame() const = 0;                                    // 12
    virtual status_t getPosition(uint32_t* position) const = 0;                 // 13
    virtual status_t getTimestamp(AudioTimestamp& ts) const = 0;                // 14
    virtual int64_t getPlayedOutDurationUs(int64_t nowUs) const = 0;            // 15
    virtual status_t getFramesWritten(uint32_t* written) const = 0;             // 16
    virtual audio_session_t getSessionId() const = 0;                           // 17
    virtual audio_stream_type_t getAudioStreamType() const = 0;                 // 18
    virtual uint32_t getSampleRate() const = 0;                                 // 19
    virtual int64_t getBufferDurationInUs() const = 0;                          // 20
    virtual audio_output_flags_t getFlags() const = 0;                          // 21
    virtual status_t open(uint32_t sampleRate, int channelCount,                // 22
                          audio_channel_mask_t channelMask, audio_format_t format,
                          int bufferCount, AudioCallback cb, void* cookie,
                          audio_output_flags_t flags, const audio_offload_info_t* offloadInfo,
                          bool doNotReconnect, uint32_t suggestedFrameCount) = 0;
    virtual status_t start() = 0;                                               // 23
    virtual ssize_t write(const void* buffer, size_t size, bool blocking) = 0;  // 24
    virtual void stop() = 0;                                                    // 25
    virtual void flush() = 0;                                                   // 26
    virtual void pause() = 0;                                                   // 27
    virtual void close() = 0;                                                   // 28
    virtual status_t setPlaybackRate(const AudioPlaybackRate& rate) = 0;        // 29
    virtual status_t getPlaybackRate(AudioPlaybackRate* rate) = 0;              // 30
    virtual bool needsTrailingPadding() = 0;                                    // 31
    virtual status_t setParameters(const String8& keyValuePairs) = 0;           // 32
    virtual String8 getParameters(const String8& keys) = 0;                     // 33
    virtual media::VolumeShaper::Status applyVolumeShaper(                      // 34
            const sp<media::VolumeShaper::Configuration>& configuration,
            const sp<media::VolumeShaper::Operation>& operation) = 0;
    virtual sp<media::VolumeShaper::State> getVolumeShaperState(int id) = 0;    // 35
    virtual status_t setOutputDevice(audio_port_handle_t deviceId) = 0;         // 36
    virtual status_t getRoutedDeviceId(audio_port_handle_t* deviceId) = 0;      // 37
    virtual status_t enableAudioDeviceCallback(bool enabled) = 0;               // 38

    // AudioSink ends here on both sides. The blob's AudioOutput carries one
    // more virtual of its own at slot 39, and MediaPlayerService::AudioOutput
    // has setHasVideo()/dump() there - neither is part of this interface, so
    // neither is reachable through it.

protected:
    virtual ~Legacy9AudioSink() {}
};

// ---------------------------------------------------------------------------
// The current AudioSink, forwarding to one of the above.

class AudioSinkBridge : public MediaPlayerBase::AudioSink {
public:
    explicit AudioSinkBridge(const sp<AudioSink>& legacy)
        : mHolder(legacy), mLegacy(reinterpret_cast<Legacy9AudioSink*>(legacy.get())) {}

    // Out of line so it is the key function: everything else here is defined in
    // the class body, and without one the vtable is never emitted.
    ~AudioSinkBridge() override;

    bool ready() const override { return mLegacy->ready(); }
    ssize_t bufferSize() const override { return mLegacy->bufferSize(); }
    ssize_t frameCount() const override { return mLegacy->frameCount(); }
    ssize_t channelCount() const override { return mLegacy->channelCount(); }
    ssize_t frameSize() const override { return mLegacy->frameSize(); }
    uint32_t latency() const override { return mLegacy->latency(); }
    float msecsPerFrame() const override { return mLegacy->msecsPerFrame(); }
    status_t getPosition(uint32_t* position) const override { return mLegacy->getPosition(position); }
    status_t getTimestamp(AudioTimestamp& ts) const override { return mLegacy->getTimestamp(ts); }
    int64_t getPlayedOutDurationUs(int64_t nowUs) const override {
        return mLegacy->getPlayedOutDurationUs(nowUs);
    }
    status_t getFramesWritten(uint32_t* written) const override {
        return mLegacy->getFramesWritten(written);
    }
    audio_session_t getSessionId() const override { return mLegacy->getSessionId(); }
    audio_stream_type_t getAudioStreamType() const override { return mLegacy->getAudioStreamType(); }
    uint32_t getSampleRate() const override { return mLegacy->getSampleRate(); }
    int64_t getBufferDurationInUs() const override { return mLegacy->getBufferDurationInUs(); }
    audio_output_flags_t getFlags() const override { return mLegacy->getFlags(); }

    status_t open(uint32_t sampleRate, int channelCount, audio_channel_mask_t channelMask,
                  audio_format_t format, int bufferCount, AudioCallback cb,
                  const wp<RefBase>& cookie, audio_output_flags_t flags,
                  const audio_offload_info_t* offloadInfo, bool doNotReconnect,
                  uint32_t suggestedFrameCount) override {
        // The blob will call back with raw pointers, so park the caller's
        // callback here and hand it a trampoline that puts the sp<>/wp<> back.
        mCallback = cb;
        mCookie = cookie;
        return mLegacy->open(sampleRate, channelCount, channelMask, format, bufferCount,
                             cb != nullptr ? &AudioSinkBridge::trampoline : nullptr, this, flags,
                             offloadInfo, doNotReconnect, suggestedFrameCount);
    }

    // Added after android 9. The blob's sink has nothing to tell, and the id is
    // only used for audio attribution, so this ends here.
    void setPlayerIId(int32_t /* playerIId */) override {}

    status_t start() override { return mLegacy->start(); }
    ssize_t write(const void* buffer, size_t size, bool blocking) override {
        return mLegacy->write(buffer, size, blocking);
    }
    void stop() override { mLegacy->stop(); }
    void flush() override { mLegacy->flush(); }
    void pause() override { mLegacy->pause(); }
    void close() override { mLegacy->close(); }
    status_t setPlaybackRate(const AudioPlaybackRate& rate) override {
        return mLegacy->setPlaybackRate(rate);
    }
    status_t getPlaybackRate(AudioPlaybackRate* rate) override {
        return mLegacy->getPlaybackRate(rate);
    }
    bool needsTrailingPadding() override { return mLegacy->needsTrailingPadding(); }
    status_t setParameters(const String8& keyValuePairs) override {
        return mLegacy->setParameters(keyValuePairs);
    }
    String8 getParameters(const String8& keys) override { return mLegacy->getParameters(keys); }
    media::VolumeShaper::Status applyVolumeShaper(
            const sp<media::VolumeShaper::Configuration>& configuration,
            const sp<media::VolumeShaper::Operation>& operation) override {
        return mLegacy->applyVolumeShaper(configuration, operation);
    }
    sp<media::VolumeShaper::State> getVolumeShaperState(int id) override {
        return mLegacy->getVolumeShaperState(id);
    }
    status_t setOutputDevice(audio_port_handle_t deviceId) override {
        return mLegacy->setOutputDevice(deviceId);
    }
    // Was getRoutedDeviceId(audio_port_handle_t*) in android 9, one id at a time.
    status_t getRoutedDeviceIds(DeviceIdVector& deviceIds) override {
        audio_port_handle_t id = AUDIO_PORT_HANDLE_NONE;
        const status_t err = mLegacy->getRoutedDeviceId(&id);
        deviceIds.clear();
        if (err == NO_ERROR && id != AUDIO_PORT_HANDLE_NONE) {
            deviceIds.push_back(id);
        }
        return err;
    }
    status_t enableAudioDeviceCallback(bool enabled) override {
        return mLegacy->enableAudioDeviceCallback(enabled);
    }

private:
    static size_t trampoline(void* /* audioSink */, void* buffer, size_t size, void* cookie,
                             Legacy9AudioSink::cb_event_t event) {
        AudioSinkBridge* const self = static_cast<AudioSinkBridge*>(cookie);
        if (self == nullptr || self->mCallback == nullptr) {
            return 0;
        }
        // The sink the caller opened is this bridge, not the blob's object.
        return self->mCallback(sp<AudioSink>::fromExisting(self), buffer, size, self->mCookie,
                               event);
    }

    const sp<AudioSink> mHolder;      // keeps the blob's sink alive
    Legacy9AudioSink* const mLegacy;  // the same object, called by android 9 slot
    AudioCallback mCallback = nullptr;
    wp<RefBase> mCookie;
};

AudioSinkBridge::~AudioSinkBridge() {}

// ---------------------------------------------------------------------------
// The android 9 NuPlayerDriver.

class LegacyNuDriver : public RefBase {
public:
    // Both out of line: the constructor is the symbol the blob looks up, and
    // the destructor is the key function that gets the vtable emitted. Left in
    // the class body they would be dropped as unused, which is exactly what a
    // build of this file did before they were moved out.
    explicit LegacyNuDriver(pid_t pid);

    virtual status_t initCheck() { return mReal->initCheck(); }                 //  6
    virtual bool hardwareOutput() { return mReal->hardwareOutput(); }           //  7
    virtual status_t setUID(uid_t uid) { return mReal->setUID(uid); }           //  8
    virtual status_t setDataSource(const sp<IMediaHTTPService>& httpService,    //  9
                                   const char* url,
                                   const KeyedVector<String8, String8>* headers) {
        return mReal->setDataSource(httpService, url, headers);
    }
    virtual status_t setDataSource(int fd, int64_t offset, int64_t length) {    // 10
        return mReal->setDataSource(fd, offset, length);
    }
    virtual status_t setDataSource(const sp<IStreamSource>& source) {           // 11
        return mReal->setDataSource(source);
    }
    // The DataSource handed in here is one of the blob's and has android 9's
    // virtual order, which is not this one - see D in docs/abi-sweep.md. It
    // reaches the right slot either way, so this is no worse than before, but
    // it still needs the same bridge treatment as the sink above.
    virtual status_t setDataSource(const sp<DataSource>& source) {              // 12
        return mReal->setDataSource(source);
    }
    virtual status_t setVideoSurfaceTexture(const sp<IGraphicBufferProducer>& bufferProducer) {
        return mReal->setVideoSurfaceTexture(bufferProducer);                   // 13
    }
    virtual status_t getBufferingSettings(BufferingSettings* buffering) {       // 14
        return mReal->getBufferingSettings(buffering);
    }
    virtual status_t setBufferingSettings(const BufferingSettings& buffering) { // 15
        return mReal->setBufferingSettings(buffering);
    }
    virtual status_t prepare() { return mReal->prepare(); }                     // 16
    virtual status_t prepareAsync() { return mReal->prepareAsync(); }           // 17
    virtual status_t start() { return mReal->start(); }                         // 18
    virtual status_t stop() { return mReal->stop(); }                           // 19
    virtual status_t pause() { return mReal->pause(); }                         // 20
    virtual bool isPlaying() { return mReal->isPlaying(); }                     // 21
    virtual status_t setPlaybackSettings(const AudioPlaybackRate& rate) {       // 22
        return mReal->setPlaybackSettings(rate);
    }
    virtual status_t getPlaybackSettings(AudioPlaybackRate* rate) {             // 23
        return mReal->getPlaybackSettings(rate);
    }
    virtual status_t setSyncSettings(const AVSyncSettings& sync, float videoFps) {
        return mReal->setSyncSettings(sync, videoFps);                          // 24
    }
    virtual status_t getSyncSettings(AVSyncSettings* sync, float* videoFps) {   // 25
        return mReal->getSyncSettings(sync, videoFps);
    }
    virtual status_t seekTo(int msec, MediaPlayerSeekMode mode) {               // 26
        return mReal->seekTo(msec, mode);
    }
    virtual status_t getCurrentPosition(int* msec) { return mReal->getCurrentPosition(msec); }
    virtual status_t getDuration(int* msec) { return mReal->getDuration(msec); }
    virtual status_t reset() { return mReal->reset(); }                         // 29
    virtual status_t notifyAt(int64_t mediaTimeUs) { return mReal->notifyAt(mediaTimeUs); }
    virtual status_t setLooping(int loop) { return mReal->setLooping(loop); }   // 31
    virtual player_type playerType() { return mReal->playerType(); }            // 32
    virtual status_t setParameter(int key, const Parcel& request) {             // 33
        return mReal->setParameter(key, request);
    }
    virtual status_t getParameter(int key, Parcel* reply) {                     // 34
        return mReal->getParameter(key, reply);
    }
    virtual status_t setRetransmitEndpoint(const struct sockaddr_in* endpoint) {
        return mReal->setRetransmitEndpoint(endpoint);                          // 35
    }
    virtual status_t getRetransmitEndpoint(struct sockaddr_in* endpoint) {      // 36
        return mReal->getRetransmitEndpoint(endpoint);
    }
    virtual status_t setNextPlayer(const sp<MediaPlayerBase>& next) {           // 37
        return mReal->setNextPlayer(next);
    }
    virtual status_t invoke(const Parcel& request, Parcel* reply) {             // 38
        return mReal->invoke(request, reply);
    }
    virtual status_t getMetadata(const media::Metadata::Filter& ids, Parcel* records) {
        return mReal->getMetadata(ids, records);                                // 39
    }
    virtual status_t dump(int fd, const Vector<String16>& args) const {         // 40
        return mReal->dump(fd, args);
    }
    virtual status_t prepareDrm(const uint8_t uuid[16], const Vector<uint8_t>& drmSessionId) {
        return mReal->prepareDrm(uuid, drmSessionId);                           // 41
    }
    virtual status_t releaseDrm() { return mReal->releaseDrm(); }               // 42
    virtual void setAudioSink(const sp<MediaPlayerBase::AudioSink>& audioSink) { // 43
        mReal->setAudioSink(audioSink != nullptr ? new AudioSinkBridge(audioSink) : nullptr);
    }

protected:
    virtual ~LegacyNuDriver();

private:
    const sp<NuPlayerDriver> mReal;
};

LegacyNuDriver::LegacyNuDriver(pid_t pid) : mReal(new NuPlayerDriver(pid)) {}

LegacyNuDriver::~LegacyNuDriver() {}

// `movs r0, #0x80` ahead of the constructor call in MobitPlayerService::
// createPlayer(). The real driver is behind a pointer, so this is nowhere near.
static_assert(sizeof(LegacyNuDriver) <= 0x80, "the blob allocates 0x80 bytes for this");

}  // namespace android
