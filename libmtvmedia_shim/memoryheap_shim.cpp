/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 * An android 9 shaped MemoryHeapBase.
 *
 * libmtv_servicejp.lge.so hands CAS payloads to its clients as MemoryBase
 * windows onto a MemoryHeapBase, and it was compiled when the class still read
 *
 *     class MemoryHeapBase : public virtual BnMemoryHeap
 *
 * Since then the virtual inheritance is gone, so MemoryHeapBase's vtable holds
 * one metadata word less and the two upcasts the blob has inlined - one to
 * RefBase for incStrong, one to IMemoryHeap to build the sp<> MemoryBase takes
 * - read displacements that no longer mean what they meant:
 *
 *              android 9                              current
 *   vptr-16    vbase offset, RefBase     (0x30)        (before the vtable)
 *   vptr-12    vbase offset, IMemoryHeap (0x20)        vbase offset, RefBase
 *
 * There is nothing to shim about a class layout, so this is not a shim in the
 * usual sense: it is that class, declared the way the blob remembers it, so the
 * displacements land on the words the blob expects. Only the constructor is
 * looked up by name - everything after that goes through the vtable this file
 * emits - so extract-files.py rewrites the single undefined symbol
 *
 *     _ZN7android14MemoryHeapBaseC1EjjPKc  ->  _ZN7android14LegacyHeapBaseC1EjjPKc
 *
 * and the blob builds these instead of libbinder's. Same length, so it is a
 * plain byte-for-byte edit of one .dynstr entry; the name is undefined in the
 * blob and .gnu.hash does not index undefined symbols, so nothing else moves.
 * Interposing libbinder's own MemoryHeapBase was the alternative and would have
 * handed every other user of it in the process an object of the wrong size.
 *
 * The object still has to fit the allocation the blob makes for it, which is
 * the android 9 sizeof - see the static_assert below. Everything the blob
 * reaches through the vtable (BnMemoryHeap::onTransact, RefBase, the IMemoryHeap
 * slots) is libbinder's and unchanged, so the heap this creates is a normal
 * binder-visible ashmem heap.
 */

#define LOG_TAG "mtv_memoryheap_shim"

#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include <binder/IMemory.h>
#include <binder/MemoryBase.h>
#include <cutils/ashmem.h>
#include <log/log.h>

namespace android {

class LegacyHeapBase : public virtual BnMemoryHeap {
public:
    explicit LegacyHeapBase(size_t size, uint32_t flags = 0, char const* name = nullptr);
    ~LegacyHeapBase() override;

    int getHeapID() const override;
    void* getBase() const override;
    size_t getSize() const override;
    uint32_t getFlags() const override;
    off_t getOffset() const override;

private:
    // The layout the blob was built against. It reads none of these - the
    // constructor and the vtable are the whole contract - but the field list is
    // what makes the virtual bases land where its inlined displacements expect,
    // and what keeps sizeof at the value it allocates.
    int mFD;
    size_t mSize;
    void* mBase;
    uint32_t mFlags;
    const char* mDevice;
    bool mNeedUnmap;
    off_t mOffset;
};

// `movs r0, #0x38` ahead of every one of the 22 constructor calls in the blob.
static_assert(sizeof(LegacyHeapBase) <= 0x38, "the blob allocates 0x38 bytes for this");

// And the MemoryBase it wraps the heap in, allocated the same way. This one is
// libbinder's, so it is only a check that the class has not grown either.
static_assert(sizeof(MemoryBase) <= 0x24, "the blob allocates 0x24 bytes for MemoryBase");

LegacyHeapBase::LegacyHeapBase(size_t size, uint32_t flags, char const* name)
    : mFD(-1), mSize(0), mBase(MAP_FAILED), mFlags(flags), mDevice(nullptr), mNeedUnmap(false),
      mOffset(0) {
    const size_t pagesize = getpagesize();
    size = (size + pagesize - 1) & ~(pagesize - 1);

    int fd = ashmem_create_region(name ? name : "LegacyHeapBase", size);
    if (fd < 0) {
        ALOGE("error creating ashmem region: %s", strerror(errno));
        return;
    }

    void* base = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        ALOGE("mmap(fd=%d, size=%zu) failed: %s", fd, size, strerror(errno));
        close(fd);
        return;
    }
    if (flags & READ_ONLY) {
        ashmem_set_prot_region(fd, PROT_READ);
    }

    mFD = fd;
    mSize = size;
    mBase = base;
    mNeedUnmap = true;
}

LegacyHeapBase::~LegacyHeapBase() {
    if (mNeedUnmap) {
        munmap(mBase, mSize);
    }
    if (mFD >= 0) {
        close(mFD);
    }
}

int LegacyHeapBase::getHeapID() const {
    return mFD;
}

void* LegacyHeapBase::getBase() const {
    return mBase;
}

size_t LegacyHeapBase::getSize() const {
    return mSize;
}

uint32_t LegacyHeapBase::getFlags() const {
    return mFlags;
}

off_t LegacyHeapBase::getOffset() const {
    return mOffset;
}

}  // namespace android
