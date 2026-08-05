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
 * links. The class is still in the framework; what is gone is the out-of-line
 * getSize(), which is now inline, and with it the vtable that used to be
 * emitted alongside it. Those two symbols are what the blob is missing.
 *
 * The virtual order is android 9's, not the current one, because it is a base
 * subobject the blob builds: DMBDataSource's vtable is baked into the blob with
 * these slot numbers. Two things moved since:
 *
 *   - getAvailableSize() was added after close(), so android 9 has 8 slots here
 *     and the current header has 9
 *   - getUri(char*, size_t) is concrete in android 9, not pure virtual
 *
 * Getting that wrong would put the destructor pair where the framework looks
 * for getAvailableSize(). It is not reachable today - the blob keeps these two
 * symbols undefined but no relocation in it points at either, so the linker
 * never resolves them - but a derived class that is handed to the framework is
 * exactly the shape of bug that stays quiet until it is not.
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
    virtual bool getUri(char* /*uriString*/, size_t /*bufferSize*/) { return false; }
    virtual uint32_t flags() { return 0; }
    virtual void close() {}

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
