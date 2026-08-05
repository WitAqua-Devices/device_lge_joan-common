/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <binder/IInterface.h>
#include <binder/IMemory.h>
#include <gui/IGraphicBufferProducer.h>
#include <utils/Errors.h>

/*
 * The full-seg binder interfaces, rebuilt from the stock libmtv_jnijp.lge.so and
 * libmtv_servicejp.lge.so. See joan-rs/DMB_PROTOCOL.md for how each transaction
 * code and parcel layout was recovered.
 *
 * Two constraints make this more than a normal header:
 *
 *  - lgemtvserver's backend (libmtv_servicejp.lge.so) is still the stock blob,
 *    and it links against this library for IDmb, IDmbService and their
 *    BnXxx::onTransact. Its DmbService::Client overrides the IDmb virtuals by
 *    slot, so the declaration order below has to match the order in the stock
 *    Client vtable exactly - getPCR really does sit between getPosition and
 *    fastForward even though its transaction code is the last one.
 *
 *  - the return types have to match what the stock proxy read back, because the
 *    blob's implementations return through them.
 */

namespace android {

/*
 * What IDmb::select takes. The JNI reads these straight off the java Dmb object
 * - the field names are in this order in the stock library's .rodata, which is
 * also the order they go into the parcel.
 */
struct SelectParam {
    int32_t opMode;
    int32_t freq;
    int32_t serviceId;
    int32_t netId;
    int32_t transportId;
    int32_t oriNetId;
    int32_t channelId;
    int32_t serviceType;
    int32_t pmtPid;
    int32_t videoPid;
    int32_t audioPid;
    int32_t emmPid;
    int32_t ecmPid;
    int32_t serviceId1Seg;
};

/* The callback numbers java switches on; they are also what the service sends
 * as the first argument of notifyCallback. */
enum {
    kSignalCallback = 0,
    kVideoCallback = 1,
    kAudioCallback = 2,
    kDataCallback = 3,
    kEventCallback = 4,
};

class IDmbClient : public IInterface {
public:
    DECLARE_META_INTERFACE(DmbClient);

    virtual void notifyCallback(int msg, int ext1, int ext2) = 0;
    virtual void dataCallback(int msg, int ext1, const sp<IMemory>& data) = 0;
};

class IDmb : public IInterface {
public:
    DECLARE_META_INTERFACE(Dmb);

    virtual void disconnect() = 0;
    virtual status_t connect(const sp<IDmbClient>& client) = 0;
    virtual void init(int mode, int arg) = 0;
    virtual void exit() = 0;
    virtual void find(int mode) = 0;
    virtual void select(SelectParam param) = 0;
    virtual void play(int mode, const char* path, int64_t position, long arg1, long arg2) = 0;
    virtual void stopPlay() = 0;
    virtual status_t record(int mode, const char* path, int64_t duration) = 0;
    virtual status_t record(int mode, int fd, int64_t duration) = 0;
    virtual void cancelRecord() = 0;
    virtual void stopRecord() = 0;
    virtual void setDataService(int type, int arg) = 0;
    virtual status_t getDuration() = 0;
    virtual void setVideoDimension(int x, int y, int width, int height, int rotation,
                                   const sp<IGraphicBufferProducer>& bufferProducer) = 0;
    virtual void setAudio(int arg1, int arg2, int arg3) = 0;
    virtual status_t setMuteStatus(int mute) = 0;
    virtual void pause() = 0;
    virtual status_t resume(int arg) = 0;
    virtual status_t pauseForBackgroundRecord(int arg) = 0;
    virtual status_t resumeForBackgroundRecord(int arg) = 0;
    virtual status_t getPosition() = 0;
    virtual int64_t getPCR() = 0;
    virtual status_t fastForward(int speed) = 0;
    virtual status_t rewind(int speed) = 0;
    virtual status_t getCASStatus() = 0;
    virtual void getDeviceID(int type) = 0;
    virtual void clearCASDB() = 0;
    virtual void setKd(int arg1, int arg2) = 0;
    virtual void autoServiceChange(long arg1, long arg2) = 0;
};

class IDmbService : public IInterface {
public:
    DECLARE_META_INTERFACE(DmbService);

    virtual sp<IDmb> connect(const sp<IDmbClient>& client) = 0;
};

class BnDmbClient : public BnInterface<IDmbClient> {
public:
    status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                        uint32_t flags = 0) override;
};

class BnDmb : public BnInterface<IDmb> {
public:
    status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                        uint32_t flags = 0) override;
};

class BnDmbService : public BnInterface<IDmbService> {
public:
    status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                        uint32_t flags = 0) override;
};

}  // namespace android
