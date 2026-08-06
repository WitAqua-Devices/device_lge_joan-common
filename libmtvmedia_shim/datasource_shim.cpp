/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 * An android 9 shaped DataSource, and the bridge that lets the current
 * framework talk to one.
 *
 * MobitPlayerService::setDataSource() hands its own DMBDataSource - a
 * DataSource subclass whose vtable is baked into the blob - to the player
 * through slot 12, which is setDataSource(const sp<DataSource>&) on both sides.
 * The pointer arrives intact; what does not survive is the virtual order behind
 * it. Two changes since android 9 move in opposite directions:
 *
 *   DataSourceBase   getAvailableSize() was added after close(), pushing the
 *                    destructor pair and everything after it down one slot.
 *
 *   DataSource       DrmInitialization() was removed - it returned
 *                    sp<DecryptHandle>, and DecryptHandle went with the legacy
 *                    DRM plugin API - pulling getUri()/getMIMEType() back up.
 *
 * So the slot count comes out the same, 14 either way, and slots 0-5 and 12-13
 * still line up. Slots 6-11 do not:
 *
 *      slot   blob (android 9)      current DataSource
 *      6      ~DMBDataSource D1     getAvailableSize
 *      7      ~DMBDataSource D0     ~DataSource D1
 *      8      getIDataSource        ~DataSource D0
 *      9      toString              getIDataSource
 *      10     reconnectAtOffset     toString
 *      11     DrmInitialization     reconnectAtOffset
 *
 * The call that actually lands is in MediaExtractorFactory::Create(), which
 * takes the remote path unless media.stagefright.extractremote is turned off:
 * CreateIDataSourceFromDataSource() -> RemoteDataSource::wrap() asks the source
 * for getIDataSource() (slot 9) and toString() (slot 10). Against the blob that
 * runs toString() and reconnectAtOffset() instead. Both of those are worse than
 * a wrong answer - getIDataSource() returns sp<IDataSource> and toString()
 * returns String8, so the String8 that toString() builds is taken for an
 * sp<IDataSource>, found non-null, and released as one; and reconnectAtOffset()
 * is handed a return-value pointer where it expects an off64_t.
 *
 * Slots 6-8 are the same shape of bug one step earlier: getAvailableSize() -
 * NuMediaExtractor.cpp reaches for it - would run the destructor, and the
 * destructor would run the deleting one. That path is not on the way here, but
 * it costs nothing to be right about it too.
 *
 * Same structure as the AudioSink pair in nuplayer_shim.cpp, with the direction
 * reversed: there the blob's object is called by the framework's slot numbers,
 * here the framework's object is called by the blob's.
 *
 *   framework --(current slots)--> DataSourceBridge --(android 9 slots)--> blob
 *
 * The orders below are not read off android 9 source. android 9 is what the
 * FileSource vtable in the phone dump's libstagefright.so emitted, current is
 * the FileSource vtable in out/.../symbols/system/lib/libdatasource.so, and the
 * blob's is the DMBDataSource vtable at 0x9666c in libmtv_servicejp.lge.so -
 * all three agree, see docs/datasource-fix.md.
 */

#define LOG_TAG "mtv_datasource_shim"

#include <media/DataSource.h>
#include <utils/Log.h>
#include <utils/String8.h>

namespace android {

// ---------------------------------------------------------------------------
// The android 9 DataSource.
//
// Never instantiated: the blob's DMBDataSource is one of these, and this
// declaration exists so its slots can be called by the numbers it was built
// with. Flat rather than DataSourceBase + virtual RefBase, because only the
// primary vtable is ever used through it and that is the part the two
// hierarchies share - DataSourceBase sits at offset 0 in both.

class Legacy9DataSource {
public:
    virtual status_t initCheck() const = 0;                                     //  0
    virtual ssize_t readAt(off64_t offset, void* data, size_t size) = 0;        //  1
    virtual status_t getSize(off64_t* size) = 0;                                //  2
    virtual bool getUri(char* uriString, size_t bufferSize) = 0;                //  3
    virtual uint32_t flags() = 0;                                               //  4
    virtual void close() = 0;                                                   //  5

protected:
    // Two slots, 6 and 7, in the middle of the run - this is the whole
    // difference. Protected as it is in the real class; nothing here ever
    // deletes through this pointer.
    virtual ~Legacy9DataSource() {}                                             //  6, 7

public:
    virtual sp<IDataSource> getIDataSource() const = 0;                         //  8
    virtual String8 toString() = 0;                                             //  9
    virtual status_t reconnectAtOffset(off64_t offset) = 0;                     // 10
    // Returned sp<DecryptHandle> in android 9. The blob inherits the default,
    // which is `return nullptr`, and nothing on the current side asks for it -
    // so this slot is only here to hold the two below in place.
    virtual void* DrmInitialization(const char* mime) = 0;                      // 11
    virtual String8 getUri() = 0;                                               // 12
    virtual String8 getMIMEType() const = 0;                                    // 13
};

// ---------------------------------------------------------------------------
// The current DataSource, forwarding to one of the above.

class DataSourceBridge : public DataSource {
public:
    explicit DataSourceBridge(const sp<DataSource>& legacy)
        : mHolder(legacy), mLegacy(reinterpret_cast<Legacy9DataSource*>(legacy.get())) {}

    // Out of line so it is the key function: everything else here is defined in
    // the class body, and without one the vtable is never emitted.
    ~DataSourceBridge() override;

    status_t initCheck() const override { return mLegacy->initCheck(); }
    ssize_t readAt(off64_t offset, void* data, size_t size) override {
        return mLegacy->readAt(offset, data, size);
    }
    status_t getSize(off64_t* size) override { return mLegacy->getSize(size); }
    uint32_t flags() override { return mLegacy->flags(); }
    void close() override { mLegacy->close(); }

    // Added after android 9, so there is nothing to forward to. -1 is what
    // DataSourceBase answers, i.e. "no idea" - the point of overriding it is
    // that the blob's slot 6 is its destructor.
    status_t getAvailableSize(off64_t /* offset */, off64_t* /* size */) override { return -1; }

    sp<IDataSource> getIDataSource() const override { return mLegacy->getIDataSource(); }
    String8 toString() override { return mLegacy->toString(); }
    status_t reconnectAtOffset(off64_t offset) override {
        return mLegacy->reconnectAtOffset(offset);
    }
    String8 getUri() override { return mLegacy->getUri(); }
    String8 getMIMEType() const override { return mLegacy->getMIMEType(); }

    // getUri(char*, size_t) is final in DataSource and cannot be overridden.
    // It does not need to be: it snprintf()s from getUri() above, which is what
    // the blob's own copy of it does with its own slot 12.

private:
    const sp<DataSource> mHolder;    // keeps the blob's source alive
    Legacy9DataSource* const mLegacy;  // the same object, called by android 9 slot
};

DataSourceBridge::~DataSourceBridge() {}

// ---------------------------------------------------------------------------

sp<DataSource> WrapLegacy9DataSource(const sp<DataSource>& legacy) {
    if (legacy == nullptr) {
        return nullptr;
    }
    ALOGI("wrapping android 9 DataSource %p", legacy.get());
    return new DataSourceBridge(legacy);
}

}  // namespace android
