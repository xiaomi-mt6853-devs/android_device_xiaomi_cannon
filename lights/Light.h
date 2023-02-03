/*
 * Copyright (C) 2023 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <aidl/android/hardware/light/BnLights.h>
#include <android-base/logging.h>
#include <hardware/hardware.h>
#include <hardware/lights.h>
#include <vector>

static unsigned int brightness_table_0x7ff[256] = {
    0,    130,  234,  314,  394,  458,  498,  553,  593,  641,  681,  697, 
    737,  777,  801,  841,  865,  881,  921,  945,  961,  1001, 1025, 1048, 
    1064, 1088, 1104, 1128, 1144, 1168, 1168, 1168, 1168, 1232, 1232, 1248, 
    1272, 1272, 1288, 1288, 1312, 1312, 1328, 1328, 1352, 1352, 1368, 1368, 
    1392, 1392, 1408, 1408, 1432, 1432, 1432, 1456, 1456, 1456, 1472, 1472, 
    1472, 1496, 1496, 1496, 1512, 1512, 1512, 1536, 1536, 1536, 1536, 1551, 
    1551, 1551, 1551, 1575, 1575, 1591, 1591, 1591, 1591, 1591, 1615, 1615, 
    1615, 1639, 1639, 1639, 1639, 1639, 1655, 1655, 1655, 1655, 1655, 1679, 
    1679, 1679, 1679, 1679, 1695, 1695, 1695, 1695, 1695, 1719, 1719, 1719, 
    1719, 1719, 1735, 1735, 1735, 1735, 1735, 1735, 1735, 1735, 1759, 1759, 
    1759, 1759, 1775, 1775, 1775, 1775, 1775, 1775, 1799, 1799, 1799, 1799, 
    1799, 1799, 1799, 1799, 1799, 1799, 1815, 1815, 1815, 1815, 1815, 1815,
    1815, 1839, 1839, 1839, 1839, 1839, 1839, 1839, 1863, 1863, 1863, 1863,
    1863, 1863, 1863, 1863, 1863, 1863, 1879, 1879, 1879, 1879, 1879, 1879,
    1879, 1879, 1879, 1903, 1903, 1903, 1903, 1903, 1903, 1903, 1903, 1903,
    1919, 1919, 1919, 1919, 1919, 1919, 1919, 1919, 1919, 1919, 1943, 1943, 
    1943, 1943, 1943, 1943, 1943, 1943, 1943, 1943, 1959, 1959, 1959, 1959,
    1959, 1959, 1959, 1959, 1959, 1959, 1959, 1959, 1983, 1983, 1983, 1983, 
    1983, 1983, 1983, 1983, 1983, 1983, 1983, 1999, 1999, 1999, 1999, 1999,
    1999, 1999, 1999, 1999, 1999, 1999, 1999, 2023, 2023, 2023, 2023, 2023, 
    2023, 2023, 2023, 2023, 2023, 2023, 2023, 2023, 2047, 2047, 2047, 2047, 
    2047, 2047, 2047, 2047
};

using ::aidl::android::hardware::light::HwLightState;
using ::aidl::android::hardware::light::HwLight;
using ::aidl::android::hardware::light::LightType;
using ::aidl::android::hardware::light::BnLights;

namespace aidl {
namespace android {
namespace hardware {
namespace light {

class Lights : public BnLights {
      ndk::ScopedAStatus setLightState(int id, const HwLightState& state) override;
      ndk::ScopedAStatus getLights(std::vector<HwLight>* types) override;
};

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
