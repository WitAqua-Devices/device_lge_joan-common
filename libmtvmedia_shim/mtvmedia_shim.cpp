/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>
#include <sys/types.h>

namespace android {

/*
 * The android 9 DataSourceBase, recreated so libmtv_servicejp.lge.so - which
 * subclasses it to feed the transport stream into the media framework - still
 * links. Only two symbols are actually referenced: the out-of-line getSize()
 * and, because getSize() is the key function, the vtable that comes with it.
 *
 * The declaration has to keep the original virtual order so the base subobject
 * a stock derived class builds still lines up. Nothing in the current framework
 * consumes a DataSourceBase, so this exists to satisfy the linker rather than
 * to be called.
 */
class DataSourceBase {
public:
    enum Flags {
        kWantsPrefetching = 1,
        kStreamedFromLocalHost = 2,
        kIsCachingDataSource = 4,
        kIsHTTPBasedSource = 8,
        kIsLocalFileSource = 16,
    };

    DataSourceBase() {}

    virtual int initCheck() const = 0;
    virtual ssize_t readAt(off64_t offset, void* data, size_t size) = 0;
    virtual int getSize(off64_t* size);
    virtual bool getUri(char* uriString, size_t bufferSize) = 0;
    virtual uint32_t flags() { return 0; }
    virtual void close() {}
    virtual int getAvailableSize(off64_t /*offset*/, off64_t* /*size*/) { return -EINVAL; }

protected:
    virtual ~DataSourceBase() {}

private:
    DataSourceBase(const DataSourceBase&);
    DataSourceBase& operator=(const DataSourceBase&);
};

int DataSourceBase::getSize(off64_t*) {
    return -EINVAL;
}

/*
 * The MIME types LG added to libstagefright_foundation next to the AOSP
 * MEDIA_MIMETYPE_* list, for the codecs ISDB-T carries. The strings are what
 * the stock library held - each symbol is a pointer, so the value was read by
 * following it into .rodata rather than guessed.
 *
 * The rest of what libmtv_servicejp references (AAC, AVC, HEVC, MPEG2, SUBRIP)
 * is still in the framework and resolves on its own.
 */
const char* MEDIA_MIMETYPE_CONTAINER_DMB = "video/dmb";
const char* MEDIA_MIMETYPE_AUDIO_BSAC = "audio/bsac";
const char* MEDIA_MIMETYPE_AUDIO_AAC_LG = "audio/mp4a-1seg";
const char* MEDIA_MIMETYPE_AUDIO_MUSICAM = "audio/MUSICAM";

}  // namespace android
