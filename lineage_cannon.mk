#
# Copyright (C) 2022 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit from cannong device
$(call inherit-product, device/xiaomi/cannon/device.mk)

PRODUCT_DEVICE := cannon
PRODUCT_NAME := lineage_cannon
PRODUCT_BRAND := Redmi
PRODUCT_MODEL := Redmi Note 9 5G
PRODUCT_MANUFACTURER := xiaomi

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="cannong-user 12 SP1A.210812.016 V13.0.2.0.SJEMIXM release-keys"

BUILD_FINGERPRINT := Redmi/cannong_global/cannong:12/SP1A.210812.016/V13.0.2.0.SJEMIXM:user/release-keys
