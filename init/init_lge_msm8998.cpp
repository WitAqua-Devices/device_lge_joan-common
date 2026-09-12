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
 * LG's own apps read the operator and the market the image was built for out
 * of these, and branch hard on them: the hidden menu builds a different list
 * of screens for KDDI, for DCM and for everyone else. vendor.prop can only
 * name one pair, and one image serves both japanese SKUs, so the pair has to
 * follow the model the bootloader reports the same way the fingerprint does.
 * ro.vendor.lge.swversion is the version string those screens then show.
 */
#define PROPERTY_LGE_OPERATOR "ro.vendor.lge.build.target_operator"
#define PROPERTY_LGE_COUNTRY "ro.vendor.lge.build.target_country"
#define PROPERTY_LGE_SWVERSION "ro.vendor.lge.swversion"

/*
 * The factory software stamp, and the pieces the same screens show next to it.
 * atd answers the version opcodes straight out of these - without them it logs
 *
 *   ro.vendor.lge.factoryversion property is not active or read error
 *   handle_get_factory_version: get_lge_factory_sw_version fail
 *
 * and the SVC and MID Info screens come up with those rows blank. The values
 * are the ones each stock build carries, so the screens read what the handset
 * left the factory with. Per SKU for the same reason as the pair above.
 */
#define PROPERTY_LGE_FACTORYVERSION "ro.vendor.lge.factoryversion"
#define PROPERTY_LGE_SWVERSION_SHORT "ro.vendor.lge.swversion_short"
#define PROPERTY_LGE_SWVERSION_SLTYPE "ro.vendor.lge.swversion_sltype"
#define PROPERTY_LGE_SWVERSION_REV "ro.vendor.lge.swversion_rev"
#define PROPERTY_LGE_SWVERSION_VENDOR "ro.vendor.lge.swversion_vendor"

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
    if(model == "LGV35") {
        variant = joan_kddi_jp_info;
        property_override(PROPERTY_LGE_OPERATOR, "KDDI");
        property_override(PROPERTY_LGE_COUNTRY, "JP");
        property_override(PROPERTY_LGE_SWVERSION, "LGV3520f");
        property_override(PROPERTY_LGE_FACTORYVERSION,
                          "LGV35HL-04-V20f-440-51-MAR-16-2020+0");
        property_override(PROPERTY_LGE_SWVERSION_SHORT, "V20f");
        property_override(PROPERTY_LGE_SWVERSION_SLTYPE, "HL");
    } else if (model == "L-01K") {
        variant = joan_dcm_jp_info;
        property_override(PROPERTY_LGE_OPERATOR, "DCM");
        property_override(PROPERTY_LGE_COUNTRY, "JP");
        property_override(PROPERTY_LGE_SWVERSION, "L01K20k");
        property_override(PROPERTY_LGE_FACTORYVERSION,
                          "LGL01KAT-00-V20k-DCM-JP-MAR-12-2020+0");
        property_override(PROPERTY_LGE_SWVERSION_SHORT, "V20k");
        property_override(PROPERTY_LGE_SWVERSION_SLTYPE, "AT");
    } else {
        /* vendor.prop already names the global pair. */
        variant = joan_global_com_info;
    }

    /* Both japanese SKUs agree on these two. */
    if (model == "LGV35" || model == "L-01K") {
        property_override(PROPERTY_LGE_SWVERSION_REV, "0");
        property_override(PROPERTY_LGE_SWVERSION_VENDOR, "LG");
    }

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
