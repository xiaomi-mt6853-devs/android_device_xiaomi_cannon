/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <libinit_utils.h>

#include <libinit_variant.h>

using android::base::GetProperty;

#define HWC_PROP "ro.boot.hwc"
#define SKU_PROP "ro.boot.product.hardware.sku"

void search_variant(const std::vector<variant_info_t> variants) {
    std::string hwc_value = GetProperty(HWC_PROP, "");
    std::string sku_value = GetProperty(SKU_PROP, "");

    for (const auto& variant : variants) {
        if ((variant.hwc_value == "" || variant.hwc_value == hwc_value) &&
            (variant.sku_value == "" || variant.sku_value == sku_value)) {
            set_variant_props(variant);
            break;
        }
    }
}

void set_variant_props(const variant_info_t variant) {
    set_ro_build_prop("brand", variant.brand, true);
    set_ro_build_prop("device", variant.device, true);
    set_ro_build_prop("model", variant.model, true);

    if (access("/system/bin/recovery", F_OK) != 0) {
        set_ro_build_prop("fingerprint", variant.build_fingerprint);
        property_override("ro.bootimage.build.fingerprint", variant.build_fingerprint);

        property_override("ro.build.description", fingerprint_to_description(variant.build_fingerprint));
    }

    property_override("ro.boot.hardware.sku", variant.device);

    if (variant.nfc)
        property_override(SKU_PROP, "nfc");

    if (variant.multisim) {
        property_override("persist.vendor.mims_support", "2");
        property_override("persist.radio.multisim.config", "dsds");
        property_override("persist.vendor.radio.msimmode", "dsds");
        property_override("persist.vendor.radio.smart.data.switch", "1");
        property_override("ro.vendor.mtk_data_config", "1");
    } else {
        property_override("persist.vendor.mims_support", "1");
        property_override("persist.radio.multisim.config", "ss");
        property_override("persist.vendor.radio.msimmode", "ss");
        property_override("persist.vendor.radio.smart.data.switch", "0");
        property_override("ro.vendor.mtk_data_config", "0");
    }

}
