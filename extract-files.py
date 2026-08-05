#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixup_remove,
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)
from extract_utils.utils import (
    run_cmd,
)

namespace_imports = [
    "device/lge/joan-common",
    "hardware/lge",
    "hardware/qcom-caf/common/libqti-perfd-client",
    "hardware/qcom-caf/msm8998",
    "hardware/qcom-caf/wlan",
    "vendor/qcom/opensource/dataservices",
    "vendor/qcom/opensource/display",
]


def lib_fixup_vendor_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'vendor' else None


lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
    (
        'com.qualcomm.qti.dpm.api@1.0',
        'vendor.qti.hardware.fm@1.0',
        'vendor.qti.hardware.qccsyshal@1.0',
        'vendor.qti.hardware.qccvndhal@1.0',
        'vendor.qti.imsrtpservice@3.0'
    ): lib_fixup_vendor_suffix,
}


blob_fixups: blob_fixups_user_type = {
    # libhidltransport was folded into libhidlbase, and only vendor still gets a
    # compat stub. lgemtvserver runs off product, where there is none.
    'product/bin/lgemtvserver': blob_fixup()
        .remove_needed('libhidltransport.so'),
    # The binary had to move to /system to reach libutils and libbinder, so the
    # path baked into the stock init script has to follow it.
    'product/etc/init/lgemtvserver.rc': blob_fixup()
        .regex_replace('/product/bin/lgemtvserver', '/system/bin/lgemtvserver'),
    # libmediaextractor is gone from the media framework, and LG's DMB MIME
    # constant went with their libstagefright_foundation. libmtvmedia_shim/ has
    # both, under its own name rather than claiming the framework's.
    #
    # The second fixup is an ABI break that cannot be shimmed. android::
    # MemoryHeapBase lost a virtual base since android 9, so its vtable now
    # carries three metadata words where it used to carry four:
    #
    #   android 9:  [vbase 0x30][vbase 0x20][offset-to-top][RTTI] vptr=+16
    #   current:    [vbase 0x2c][offset-to-top][RTTI]             vptr=+12
    #
    # Every sp<MemoryHeapBase> in here reaches RefBase with the offset baked in
    # at build time - `ldr rN,[vptr,#-16]` - which now reads past the start of
    # the vtable and adds a relocated pointer to the object, so the first CAS
    # callback takes lgemtvserver down with SIGSEGV in RefBase::incStrong.
    # Rewriting the displacement to -12 is the whole fix; the three-instruction
    # idiom is matched so nothing else with a -16 displacement is touched, and
    # MemoryBase/IMemory/IMemoryHeap kept their layout and use -12 already, so
    # they never match. 24 sites, one byte each.
    'system/lib/libmtv_servicejp.lge.so': blob_fixup()
        .replace_needed('libmediaextractor.so', 'libmtvmedia_shim.so')
        .binary_regex_replace(
            rb'(?s)([\x50-\x5f]\xf8)\x10'
            rb'([\x0c\x1c\x2c\x3c\x4c\x5c\x6c\x7c\x8c\x9c\xac\xbc\xcc\xdc\xec\xfc])(.\x44)',
            b'\\g<1>' + b'\x0c' + b'\\g<2>\\g<3>'),
    # Same for the shared library declaration: the jar moved to /system, and the
    # path in here is what decides which linker namespace the app gets when it
    # loads the jar's JNI - product-clns cannot see libcutils.
    'system/etc/permissions/com.lge.broadcast.jfullseg.xml': blob_fixup()
        .regex_replace('/product/framework/', '/system/framework/'),
    'system_ext/lib64/lib-imscamera.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system_ext/lib64/lib-imsvideocodec.so': blob_fixup()
        .add_needed('libgui_shim.so')
        .replace_needed('libqdMetaData.so', 'libqdMetaData.system.so'),
    'vendor/lib/hw/camera.msm8998.so': blob_fixup()
        .remove_needed('libgui.so')
        .remove_needed('libsensor.so')
        .remove_needed('libui.so')
        .binary_regex_replace(b'libandroid.so', b'libui_shim.so'),
    'vendor/lib/libarcsoft_beauty_picselfie.so': blob_fixup()
        .remove_needed('libandroid.so')
        .remove_needed('libjnigraphics.so')
        .replace_needed('libstdc++.so', 'libstdc++_vendor.so'),
    'vendor/lib/libfilm_emulation.so': blob_fixup()
        .remove_needed('libjnigraphics.so')
        .replace_needed('libstdc++.so', 'libstdc++_vendor.so'),
    'vendor/lib/libmmcamera_bokeh.so': blob_fixup()
        .replace_needed('libui.so', 'libui_shim.so'),
    'vendor/lib/libmpbase.so': blob_fixup()
        .remove_needed('libandroid.so')
        .replace_needed('libstdc++.so', 'libstdc++_vendor.so'),
    (
        'vendor/lib/libAutoContrast.so',
        'vendor/lib/libSJFingerDetect.so',
        'vendor/lib/libSJVideoNR.so',
        'vendor/lib/libarcsoft_object_tracking.so',
        'vendor/lib/libarcsoft_picselfie_algorithm.so',
        'vendor/lib/libcinemaeffect.so',
        'vendor/lib/libfilm_emulation_symphony.so',
        'vendor/lib/liblghdri.so',
        'vendor/lib/liblgmda.so',
        'vendor/lib/libmorpho_image_stab31.so',
        'vendor/lib/libmorpho_superzoom.so'
    ): blob_fixup()
        .replace_needed('libstdc++.so', 'libstdc++_vendor.so'),
    'vendor/lib/sensors.ssc.so': blob_fixup()
        .sig_replace('11 F0 9E F8', '4D D2 00 BF'),
    'vendor/lib64/sensors.ssc.so': blob_fixup()
        .sig_replace('F2 8B FF 97', '2A 00 00 14'),
    'vendor/lib64/libwvhidl.so': blob_fixup()
        .add_needed('libcrypto_shim.so'),
    'vendor/lib/libmmcamera_faceproc.so': blob_fixup()
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),
}  # fmt: skip

module = ExtractUtilsModule(
    'joan-common',
    'lge',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
