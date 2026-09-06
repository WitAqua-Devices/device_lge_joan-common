/*
    Copyright (C) 2007, The Android Open Source Project
    Copyright (c) 2016, The CyanogenMod Project
    Copyright (c) 2017-2024, The LineageOS Project

    SPDX-License-Identifier: BSD-3-Clause
 */

#include <android-base/logging.h>
#include <android-base/properties.h>

#include "vendor_init.h"

#include "libinit_utils.h"
#include "libinit_variant.h"

using android::base::GetProperty;

#define PROPERTY_LGE_MODEL "ro.boot.vendor.lge.model.name"

/*
 * The stock ro.product.name - joan_dcm_jp and friends - is the one spelling of
 * the SKU that everything else already uses: the kernel device trees, the
 * defconfig symbols, and the per-variant copies of the calibration data that
 * init.joan.rc binds over the canonical paths. Publishing it here keeps the rc
 * file from carrying a second copy of the model-to-variant table.
 */
#define PROPERTY_LGE_SKU "ro.vendor.lge.sku"

/*
 * The model in each variant is the one the stock build reports, and doubles as
 * the fallback for when the bootloader does not hand us PROPERTY_LGE_MODEL:
 * an unknown SKU is still better described by the identity we are about to
 * spoof than by a placeholder.
 */
static variant_info_t joan_global_com_info = {
    .brand = "lge",
    .device = "joan",
    .model = "LG-H930",
    .build_fingerprint = "lge/joan_global_com/joan:8.0.0/OPR1.170623.026/181381736b4e9:user/release-keys",
};

static variant_info_t joan_kddi_jp_info = {
    .brand = "KDDI",
    .device = "joan",
    .model = "LGV35",
    .build_fingerprint = "KDDI/joan_kddi_jp/joan:9/PKQ1.190414.001/2007610508923:user/release-keys",
};

static variant_info_t joan_dcm_jp_info = {
    .brand = "lge",
    .device = "L-01K",
    .model = "L-01K",
    .build_fingerprint = "lge/joan_dcm_jp/L-01K:9/PKQ1.190414.001/20072184679ed:user/release-keys",
};

/*
 * The product name in the fingerprint - joan_dcm_jp and friends - is the stock
 * ro.product.name. set_variant_props() does not touch it, so without this
 * build.prop keeps saying lineage_<device> while the fingerprint claims
 * otherwise, and anything comparing the two sees a device that does not add
 * up. Read it back out of the fingerprint rather than keeping a second table
 * in sync with it.
 */
static std::string fingerprint_to_name(const std::string &fingerprint)
{
    size_t begin = fingerprint.find('/');
    if (begin == std::string::npos)
        return {};

    size_t end = fingerprint.find('/', begin + 1);
    if (end == std::string::npos)
        return {};

    return fingerprint.substr(begin + 1, end - begin - 1);
}

void init_target_properties()
{
    std::string model;
    variant_info_t variant;

    if(parse_cmdline("lge.dsds") == "dsds")
    {
        property_override("persist.radio.multisim.config", "dsds");
    }

    model = GetProperty(PROPERTY_LGE_MODEL, "");
    if(model == "LGV35")
        variant = joan_kddi_jp_info;
    else if (model == "L-01K")
        variant = joan_dcm_jp_info;
    else
        variant = joan_global_com_info;

    /* Only the bootloader knows the SKU; keep the variant model without it. */
    if (!model.empty())
        variant.model = model;

    set_variant_props(variant);

    std::string name = fingerprint_to_name(variant.build_fingerprint);
    set_ro_build_prop("name", name, true);
    property_override(PROPERTY_LGE_SKU, name);
}

void vendor_load_properties() {
    LOG(INFO) << "Loading vendor specific properties";
    init_target_properties();
}
