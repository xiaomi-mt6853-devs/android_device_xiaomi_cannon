/*
 * Copyright (C) 2022 bengris32
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

extern "C" {

// undefined4 libpowerhal_Init(int param_1)
void libpowerhal_Init(int param_1);

// aidl::android::hardware::power::impl::mediatek::Power::setBoost
//          (Boost type, int32_t durationMs)
// __pid_t pid = getpid();
// libpowerhal_wrap_CusLockHint(type,durationMs,pid);
int libpowerhal_CusLockHint(int32_t type, int32_t durationMs, __pid_t pid);

// undefined8 libpowerhal_LockRel(int param_1)
void libpowerhal_LockRel(int handle);

// undefined8 libpowerhal_UserScnDisableAll(void)
void libpowerhal_UserScnDisableAll();

// undefined8 libpowerhal_UserScnRestoreAll(void)
void libpowerhal_UserScnRestoreAll();

}  // extern "c"
