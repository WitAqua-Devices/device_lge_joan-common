/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "DmbJP"

#define DO_NOT_CHECK_MANUAL_BINDER_INTERFACES

#include "IDmb.h"

#include <binder/Parcel.h>
#include <log/log.h>

/*
 * Both halves of the wire protocol. The proxies are what the app's JNI uses;
 * the onTransact implementations are what lgemtvserver's stock backend uses,
 * since that blob links against this library rather than carrying its own copy.
 */

namespace android {

// Transaction codes, as read out of the stock proxies. 8 is not used - the java
// side calls native_play_v3, so an earlier play presumably had it.
enum {
    DISCONNECT = IBinder::FIRST_CALL_TRANSACTION,  // 1
    CONNECT,                                       // 2
    INIT,                                          // 3
    EXIT,                                          // 4
    FIND,                                          // 5
    SELECT,                                        // 6
    PLAY,                                          // 7
    STOP_PLAY = 9,
    RECORD,                                        // 10
    RECORD_FD,                                     // 11
    CANCEL_RECORD,                                 // 12
    STOP_RECORD,                                   // 13
    SET_DATA_SERVICE,                              // 14
    GET_DURATION,                                  // 15
    SET_VIDEO_DIMENSION,                           // 16
    SET_AUDIO,                                     // 17
    SET_MUTE_STATUS,                               // 18
    PAUSE,                                         // 19
    RESUME,                                        // 20
    PAUSE_FOR_BACKGROUND_RECORD,                   // 21
    RESUME_FOR_BACKGROUND_RECORD,                  // 22
    GET_POSITION,                                  // 23
    FAST_FORWARD,                                  // 24
    REWIND,                                        // 25
    GET_CAS_STATUS,                                // 26
    GET_DEVICE_ID,                                 // 27
    CLEAR_CAS_DB,                                  // 28
    SET_KD,                                        // 29
    AUTO_SERVICE_CHANGE,                           // 30
    GET_PCR,                                       // 31
};

enum {
    SERVICE_CONNECT = IBinder::FIRST_CALL_TRANSACTION,  // 1
};

enum {
    NOTIFY_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,  // 1
    DATA_CALLBACK,                                      // 2
};

// ---------------------------------------------------------------------------
// IDmbClient

class BpDmbClient : public BpInterface<IDmbClient> {
public:
    explicit BpDmbClient(const sp<IBinder>& impl) : BpInterface<IDmbClient>(impl) {}

    void notifyCallback(int msg, int ext1, int ext2) override {
        Parcel data, reply;
        data.writeInterfaceToken(IDmbClient::getInterfaceDescriptor());
        data.writeInt32(msg);
        data.writeInt32(ext1);
        data.writeInt32(ext2);
        remote()->transact(NOTIFY_CALLBACK, data, &reply);
    }

    void dataCallback(int msg, int ext1, const sp<IMemory>& mem) override {
        Parcel data, reply;
        data.writeInterfaceToken(IDmbClient::getInterfaceDescriptor());
        data.writeInt32(msg);
        data.writeInt32(ext1);
        data.writeStrongBinder(IInterface::asBinder(mem));
        remote()->transact(DATA_CALLBACK, data, &reply);
    }
};

IMPLEMENT_META_INTERFACE(DmbClient, "android.broadcast.IDmbClient");

status_t BnDmbClient::onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                                 uint32_t flags) {
    switch (code) {
        case NOTIFY_CALLBACK: {
            CHECK_INTERFACE(IDmbClient, data, reply);
            int msg = data.readInt32();
            int ext1 = data.readInt32();
            int ext2 = data.readInt32();
            notifyCallback(msg, ext1, ext2);
            return NO_ERROR;
        }
        case DATA_CALLBACK: {
            CHECK_INTERFACE(IDmbClient, data, reply);
            int msg = data.readInt32();
            int ext1 = data.readInt32();
            sp<IMemory> mem = interface_cast<IMemory>(data.readStrongBinder());
            dataCallback(msg, ext1, mem);
            return NO_ERROR;
        }
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

// ---------------------------------------------------------------------------
// IDmb

class BpDmb : public BpInterface<IDmb> {
public:
    explicit BpDmb(const sp<IBinder>& impl) : BpInterface<IDmb>(impl) {}

    // Every call starts the same way, so the boilerplate lives here. Parcel is
    // not copyable, so this fills one in place rather than handing one back.
    void begin(Parcel& data) { data.writeInterfaceToken(IDmb::getInterfaceDescriptor()); }

    void call(uint32_t code, Parcel& data) {
        Parcel reply;
        remote()->transact(code, data, &reply);
    }

    status_t callInt(uint32_t code, Parcel& data) {
        Parcel reply;
        remote()->transact(code, data, &reply);
        return reply.readInt32();
    }

    void disconnect() override {
        Parcel data;
        begin(data);
        call(DISCONNECT, data);
    }

    status_t connect(const sp<IDmbClient>& client) override {
        Parcel data;
        begin(data);
        data.writeStrongBinder(IInterface::asBinder(client));
        return callInt(CONNECT, data);
    }

    void init(int mode, int arg) override {
        Parcel data;
        begin(data);
        data.writeInt32(mode);
        data.writeInt32(arg);
        call(INIT, data);
    }

    void exit() override {
        Parcel data;
        begin(data);
        call(EXIT, data);
    }

    void find(int mode) override {
        Parcel data;
        begin(data);
        data.writeInt32(mode);
        call(FIND, data);
    }

    void select(SelectParam p) override {
        Parcel data;
        begin(data);
        data.writeInt32(p.opMode);
        data.writeInt32(p.freq);
        data.writeInt32(p.serviceId);
        data.writeInt32(p.netId);
        data.writeInt32(p.transportId);
        data.writeInt32(p.oriNetId);
        data.writeInt32(p.channelId);
        data.writeInt32(p.serviceType);
        data.writeInt32(p.pmtPid);
        data.writeInt32(p.videoPid);
        data.writeInt32(p.audioPid);
        data.writeInt32(p.emmPid);
        data.writeInt32(p.ecmPid);
        data.writeInt32(p.serviceId1Seg);
        call(SELECT, data);
    }

    void play(int mode, const char* path, int64_t position, long arg1, long arg2) override {
        Parcel data;
        begin(data);
        data.writeInt32(mode);
        data.writeCString(path);
        data.writeInt64(position);
        data.writeInt32(static_cast<int32_t>(arg1));
        data.writeInt32(static_cast<int32_t>(arg2));
        call(PLAY, data);
    }

    void stopPlay() override {
        Parcel data;
        begin(data);
        call(STOP_PLAY, data);
    }

    status_t record(int mode, const char* path, int64_t duration) override {
        Parcel data;
        begin(data);
        data.writeInt32(mode);
        data.writeCString(path);
        data.writeInt64(duration);
        return callInt(RECORD, data);
    }

    status_t record(int mode, int fd, int64_t duration) override {
        Parcel data;
        begin(data);
        data.writeInt32(mode);
        data.writeFileDescriptor(fd);
        data.writeInt64(duration);
        return callInt(RECORD_FD, data);
    }

    void cancelRecord() override {
        Parcel data;
        begin(data);
        call(CANCEL_RECORD, data);
    }

    void stopRecord() override {
        Parcel data;
        begin(data);
        call(STOP_RECORD, data);
    }

    void setDataService(int type, int arg) override {
        Parcel data;
        begin(data);
        data.writeInt32(type);
        data.writeInt32(arg);
        call(SET_DATA_SERVICE, data);
    }

    status_t getDuration() override {
        Parcel data;
        begin(data);
        return callInt(GET_DURATION, data);
    }

    void setVideoDimension(int x, int y, int width, int height, int rotation,
                           const sp<IGraphicBufferProducer>& bufferProducer) override {
        Parcel data;
        begin(data);
        data.writeInt32(x);
        data.writeInt32(y);
        data.writeInt32(width);
        data.writeInt32(height);
        data.writeInt32(rotation);
        data.writeStrongBinder(IInterface::asBinder(bufferProducer));
        call(SET_VIDEO_DIMENSION, data);
    }

    void setAudio(int arg1, int arg2, int arg3) override {
        Parcel data;
        begin(data);
        data.writeInt32(arg1);
        data.writeInt32(arg2);
        data.writeInt32(arg3);
        call(SET_AUDIO, data);
    }

    status_t setMuteStatus(int mute) override {
        Parcel data;
        begin(data);
        data.writeInt32(mute);
        return callInt(SET_MUTE_STATUS, data);
    }

    void pause() override {
        Parcel data;
        begin(data);
        call(PAUSE, data);
    }

    status_t resume(int arg) override {
        Parcel data;
        begin(data);
        data.writeInt32(arg);
        return callInt(RESUME, data);
    }

    status_t pauseForBackgroundRecord(int arg) override {
        Parcel data;
        begin(data);
        data.writeInt32(arg);
        return callInt(PAUSE_FOR_BACKGROUND_RECORD, data);
    }

    status_t resumeForBackgroundRecord(int arg) override {
        Parcel data;
        begin(data);
        data.writeInt32(arg);
        return callInt(RESUME_FOR_BACKGROUND_RECORD, data);
    }

    status_t getPosition() override {
        Parcel data;
        begin(data);
        return callInt(GET_POSITION, data);
    }

    int64_t getPCR() override {
        Parcel data, reply;
        begin(data);
        remote()->transact(GET_PCR, data, &reply);
        return reply.readInt64();
    }

    status_t fastForward(int speed) override {
        Parcel data;
        begin(data);
        data.writeInt32(speed);
        return callInt(FAST_FORWARD, data);
    }

    status_t rewind(int speed) override {
        Parcel data;
        begin(data);
        data.writeInt32(speed);
        return callInt(REWIND, data);
    }

    status_t getCASStatus() override {
        Parcel data;
        begin(data);
        return callInt(GET_CAS_STATUS, data);
    }

    void getDeviceID(int type) override {
        Parcel data;
        begin(data);
        data.writeInt32(type);
        call(GET_DEVICE_ID, data);
    }

    void clearCASDB() override {
        Parcel data;
        begin(data);
        call(CLEAR_CAS_DB, data);
    }

    void setKd(int arg1, int arg2) override {
        Parcel data;
        begin(data);
        data.writeInt32(arg1);
        data.writeInt32(arg2);
        call(SET_KD, data);
    }

    void autoServiceChange(long arg1, long arg2) override {
        Parcel data;
        begin(data);
        data.writeInt32(static_cast<int32_t>(arg1));
        data.writeInt32(static_cast<int32_t>(arg2));
        call(AUTO_SERVICE_CHANGE, data);
    }
};

IMPLEMENT_META_INTERFACE(Dmb, "android.broadcast.IDmb");

status_t BnDmb::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
    switch (code) {
        case DISCONNECT: {
            CHECK_INTERFACE(IDmb, data, reply);
            disconnect();
            return NO_ERROR;
        }
        case CONNECT: {
            CHECK_INTERFACE(IDmb, data, reply);
            sp<IDmbClient> client = interface_cast<IDmbClient>(data.readStrongBinder());
            reply->writeInt32(connect(client));
            return NO_ERROR;
        }
        case INIT: {
            CHECK_INTERFACE(IDmb, data, reply);
            int mode = data.readInt32();
            int arg = data.readInt32();
            init(mode, arg);
            return NO_ERROR;
        }
        case EXIT: {
            CHECK_INTERFACE(IDmb, data, reply);
            exit();
            return NO_ERROR;
        }
        case FIND: {
            CHECK_INTERFACE(IDmb, data, reply);
            find(data.readInt32());
            return NO_ERROR;
        }
        case SELECT: {
            CHECK_INTERFACE(IDmb, data, reply);
            SelectParam p;
            p.opMode = data.readInt32();
            p.freq = data.readInt32();
            p.serviceId = data.readInt32();
            p.netId = data.readInt32();
            p.transportId = data.readInt32();
            p.oriNetId = data.readInt32();
            p.channelId = data.readInt32();
            p.serviceType = data.readInt32();
            p.pmtPid = data.readInt32();
            p.videoPid = data.readInt32();
            p.audioPid = data.readInt32();
            p.emmPid = data.readInt32();
            p.ecmPid = data.readInt32();
            p.serviceId1Seg = data.readInt32();
            select(p);
            return NO_ERROR;
        }
        case PLAY: {
            CHECK_INTERFACE(IDmb, data, reply);
            int mode = data.readInt32();
            const char* path = data.readCString();
            int64_t position = data.readInt64();
            long arg1 = data.readInt32();
            long arg2 = data.readInt32();
            play(mode, path, position, arg1, arg2);
            return NO_ERROR;
        }
        case STOP_PLAY: {
            CHECK_INTERFACE(IDmb, data, reply);
            stopPlay();
            return NO_ERROR;
        }
        case RECORD: {
            CHECK_INTERFACE(IDmb, data, reply);
            int mode = data.readInt32();
            const char* path = data.readCString();
            int64_t duration = data.readInt64();
            reply->writeInt32(record(mode, path, duration));
            return NO_ERROR;
        }
        case RECORD_FD: {
            CHECK_INTERFACE(IDmb, data, reply);
            int mode = data.readInt32();
            int fd = data.readFileDescriptor();
            int64_t duration = data.readInt64();
            reply->writeInt32(record(mode, fd, duration));
            return NO_ERROR;
        }
        case CANCEL_RECORD: {
            CHECK_INTERFACE(IDmb, data, reply);
            cancelRecord();
            return NO_ERROR;
        }
        case STOP_RECORD: {
            CHECK_INTERFACE(IDmb, data, reply);
            stopRecord();
            return NO_ERROR;
        }
        case SET_DATA_SERVICE: {
            CHECK_INTERFACE(IDmb, data, reply);
            int type = data.readInt32();
            int arg = data.readInt32();
            setDataService(type, arg);
            return NO_ERROR;
        }
        case GET_DURATION: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(getDuration());
            return NO_ERROR;
        }
        case SET_VIDEO_DIMENSION: {
            CHECK_INTERFACE(IDmb, data, reply);
            int x = data.readInt32();
            int y = data.readInt32();
            int width = data.readInt32();
            int height = data.readInt32();
            int rotation = data.readInt32();
            sp<IGraphicBufferProducer> bp =
                    interface_cast<IGraphicBufferProducer>(data.readStrongBinder());
            setVideoDimension(x, y, width, height, rotation, bp);
            return NO_ERROR;
        }
        case SET_AUDIO: {
            CHECK_INTERFACE(IDmb, data, reply);
            int a = data.readInt32();
            int b = data.readInt32();
            int c = data.readInt32();
            setAudio(a, b, c);
            return NO_ERROR;
        }
        case SET_MUTE_STATUS: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(setMuteStatus(data.readInt32()));
            return NO_ERROR;
        }
        case PAUSE: {
            CHECK_INTERFACE(IDmb, data, reply);
            pause();
            return NO_ERROR;
        }
        case RESUME: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(resume(data.readInt32()));
            return NO_ERROR;
        }
        case PAUSE_FOR_BACKGROUND_RECORD: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(pauseForBackgroundRecord(data.readInt32()));
            return NO_ERROR;
        }
        case RESUME_FOR_BACKGROUND_RECORD: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(resumeForBackgroundRecord(data.readInt32()));
            return NO_ERROR;
        }
        case GET_POSITION: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(getPosition());
            return NO_ERROR;
        }
        case FAST_FORWARD: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(fastForward(data.readInt32()));
            return NO_ERROR;
        }
        case REWIND: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(rewind(data.readInt32()));
            return NO_ERROR;
        }
        case GET_CAS_STATUS: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt32(getCASStatus());
            return NO_ERROR;
        }
        case GET_DEVICE_ID: {
            CHECK_INTERFACE(IDmb, data, reply);
            getDeviceID(data.readInt32());
            return NO_ERROR;
        }
        case CLEAR_CAS_DB: {
            CHECK_INTERFACE(IDmb, data, reply);
            clearCASDB();
            return NO_ERROR;
        }
        case SET_KD: {
            CHECK_INTERFACE(IDmb, data, reply);
            int a = data.readInt32();
            int b = data.readInt32();
            setKd(a, b);
            return NO_ERROR;
        }
        case AUTO_SERVICE_CHANGE: {
            CHECK_INTERFACE(IDmb, data, reply);
            long a = data.readInt32();
            long b = data.readInt32();
            autoServiceChange(a, b);
            return NO_ERROR;
        }
        case GET_PCR: {
            CHECK_INTERFACE(IDmb, data, reply);
            reply->writeInt64(getPCR());
            return NO_ERROR;
        }
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

// ---------------------------------------------------------------------------
// IDmbService

class BpDmbService : public BpInterface<IDmbService> {
public:
    explicit BpDmbService(const sp<IBinder>& impl) : BpInterface<IDmbService>(impl) {}

    sp<IDmb> connect(const sp<IDmbClient>& client) override {
        Parcel data, reply;
        data.writeInterfaceToken(IDmbService::getInterfaceDescriptor());
        data.writeStrongBinder(IInterface::asBinder(client));
        remote()->transact(SERVICE_CONNECT, data, &reply);
        return interface_cast<IDmb>(reply.readStrongBinder());
    }
};

IMPLEMENT_META_INTERFACE(DmbService, "android.broadcast.IDmbService");

status_t BnDmbService::onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                                  uint32_t flags) {
    switch (code) {
        case SERVICE_CONNECT: {
            CHECK_INTERFACE(IDmbService, data, reply);
            sp<IDmbClient> client = interface_cast<IDmbClient>(data.readStrongBinder());
            sp<IDmb> dmb = connect(client);
            reply->writeStrongBinder(IInterface::asBinder(dmb));
            return NO_ERROR;
        }
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}  // namespace android
