#!/bin/bash
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

set -e

DEVICE=cannon
VENDOR=xiaomi

# Load extract_utils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${MY_DIR}" ]]; then MY_DIR="${PWD}"; fi

ANDROID_ROOT="${MY_DIR}/../../.."

HELPER="${ANDROID_ROOT}/tools/extract-utils/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

# Default to sanitizing the vendor folder before extraction
CLEAN_VENDOR=true

KANG=
SECTION=

while [ "${#}" -gt 0 ]; do
    case "${1}" in
        -n | --no-cleanup )
                CLEAN_VENDOR=false
                ;;
        -k | --kang )
                KANG="--kang"
                ;;
        -s | --section )
                SECTION="${2}"; shift
                CLEAN_VENDOR=false
                ;;
        * )
                SRC="${1}"
                ;;
    esac
    shift
done

if [ -z "${SRC}" ]; then
    SRC="adb"
fi

function blob_fixup() {
    case "${1}" in
    vendor/bin/hw/android.hardware.thermal@2.0-service.mtk)
        "${PATCHELF}" --replace-needed "libutils.so" "libutils-v32.so" "${2}"
        "${PATCHELF}" --replace-needed "libhidlbase.so" "libhidlbase-v32.so" "${2}"
        ;;
    vendor/bin/hw/camerahalserver)
        "${PATCHELF}" --replace-needed "libutils.so" "libutils-v32.so" "${2}"
        "${PATCHELF}" --replace-needed "libbinder.so" "libbinder-v32.so" "${2}"
        "${PATCHELF}" --replace-needed "libhidlbase.so" "libhidlbase-v32.so" "${2}"
        ;;
    vendor/bin/mtk_agpsd)
       "$PATCHELF" --replace-needed "libcrypto.so" "libcrypto-v32.so" "$2"
        ;;
    vendor/lib*/hw/vendor.mediatek.hardware.pq@2.13-impl.so)
        "${PATCHELF}" --replace-needed "libutils.so" "libutils-v32.so" "${2}"
        "${PATCHELF}" --add-needed "libshim_sensors.so" "${2}"
        ;;
    vendor/lib64/libmtkcam_featurepolicy.so)
        sed -i "s|\xE8\x87\x40\xB9|\x28\x02\x80\x52|g" "${2}"
        ;;
    vendor/bin/mnld|\
    vendor/lib64/libaalservice.so|\
    vendor/lib64/libcam.utils.sensorprovider.so)
        "${PATCHELF}" --add-needed "libshim_sensors.so" "${2}"
        ;;
    vendor/bin/hw/android.hardware.media.c2@1.2-mediatek)
        "${PATCHELF}" --add-needed "libstagefright_foundation-v33.so" "${2}"
        ;;
    vendor/etc/init/vendor.mediatek.hardware.mtkpower@1.0-service.rc)
        echo "$(cat ${2}) input" > "${2}"
        ;;
    esac
}

# Initialize the helper
setup_vendor "${DEVICE}" "${VENDOR}" "${ANDROID_ROOT}" false "${CLEAN_VENDOR}"

extract "${MY_DIR}/proprietary-files.txt" "${SRC}" "${KANG}" --section "${SECTION}"

"${MY_DIR}/setup-makefiles.sh"
