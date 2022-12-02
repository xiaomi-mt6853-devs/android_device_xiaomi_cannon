/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libinit_dalvik_heap.h>
#include <libinit_variant.h>

#include "vendor_init.h"

static const variant_info_t cannon_info = {
    .hwc_value = "CN",
    .sku_value = "",

    .brand = "Xiaomi",
    .device = "cannon",
    .model = "M2007J22C",
    .build_fingerprint = "Redmi/cannon/cannon:12/SP1A.210812.016/V13.0.3.0.SJECNXM:user/release-keys",

    .nfc = false,
};

static const variant_info_t cannong_info = {
    .hwc_value = "GL",
    .sku_value = "",

    .brand = "Xiaomi",
    .device = "cannong",
    .model = "M2007J22G",
    .build_fingerprint = "Redmi/cannong_global/cannong:12/SP1A.210812.016/V13.0.3.0.SJEMIXM:user/release-keys",

    .nfc = true,
};

static const variant_info_t cannong_jp_info = {
    .hwc_value = "JP",
    .sku_value = "",

    .brand = "Xiaomi",
    .device = "cannong",
    .model = "A001XM",
    .build_fingerprint = "Redmi/A001XM/cannong:12/SP1A.210812.016/V13.0.2.0.SJEJPSB:user/release-keys",

    .nfc = true,
};

static const std::vector<variant_info_t> variants = {
    cannon_info,
    cannong_info,
    cannong_jp_info,
};

void vendor_load_properties() {
    search_variant(variants);
    set_dalvik_heap();
}
