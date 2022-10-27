/*
 * Copyright (C) 2020 The Android Open Source Project
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

#include "Power.h"

#include <android-base/file.h>
#include <android-base/logging.h>

#include <linux/input.h>

constexpr int kWakeupModeOff = 4;
constexpr int kWakeupModeOn = 5;

#include <dlfcn.h>

#define POWERHAL_LIB_NAME "libpowerhal.so"
#define LIBPOWERHAL_INIT "libpowerhal_Init"
#define LIBPOWERHAL_CUSLOCKHINT "libpowerhal_CusLockHint"
#define LIBPOWERHAL_LOCKREL "libpowerhal_LockRel"
#define LIBPOWERHAL_USERSCNDISABLEALL "libpowerhal_UserScnDisableAll"
#define LIBPOWERHAL_USERSCNRESTOREALL "libpowerhal_UserScnRestoreAll"

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {
namespace mediatek {

const std::vector<Boost> BOOST_RANGE{ndk::enum_range<Boost>().begin(),
                                     ndk::enum_range<Boost>().end()};
const std::vector<Mode> MODE_RANGE{ndk::enum_range<Mode>().begin(), ndk::enum_range<Mode>().end()};

Power::Power() {
    powerHandle = dlopen(POWERHAL_LIB_NAME, RTLD_LAZY);
    if (!powerHandle) {
        LOG(ERROR) << "Could not dlopen " << POWERHAL_LIB_NAME;
        abort();
    }

    libpowerhal_Init =
        reinterpret_cast<libpowerhal_Init_handle>(dlsym(powerHandle, LIBPOWERHAL_INIT));

    if (!libpowerhal_Init) {
        LOG(ERROR) << "Could not locate symbol " << LIBPOWERHAL_INIT;
        abort();
    }

    libpowerhal_CusLockHint =
        reinterpret_cast<libpowerhal_CusLockHint_handle>(dlsym(powerHandle, LIBPOWERHAL_CUSLOCKHINT));

    if (!libpowerhal_CusLockHint) {
        LOG(ERROR) << "Could not locate symbol " << LIBPOWERHAL_CUSLOCKHINT;
        abort();
    }

    libpowerhal_LockRel =
        reinterpret_cast<libpowerhal_LockRel_handle>(dlsym(powerHandle, LIBPOWERHAL_LOCKREL));

    if (!libpowerhal_LockRel) {
        LOG(ERROR) << "Could not locate symbol " << LIBPOWERHAL_LOCKREL;
        abort();
    }

    libpowerhal_UserScnDisableAll =
         reinterpret_cast<libpowerhal_UserScnDisableAll_handle>(dlsym(powerHandle, LIBPOWERHAL_USERSCNDISABLEALL));

    if (!libpowerhal_UserScnDisableAll) {
        LOG(ERROR) << "Could not locate symbol " << LIBPOWERHAL_USERSCNDISABLEALL;
        abort();
    }

    libpowerhal_UserScnRestoreAll =
        reinterpret_cast<libpowerhal_UserScnRestoreAll_handle>(dlsym(powerHandle, LIBPOWERHAL_USERSCNRESTOREALL));

    if (!libpowerhal_UserScnRestoreAll) {
        LOG(ERROR) << "Could not locate symbol " << LIBPOWERHAL_USERSCNRESTOREALL;
        abort();
    }

    libpowerhal_Init(1);
}

Power::~Power() { }

static int open_ts_input() {
    int fd = -1;
    DIR *dir = opendir("/dev/input");

    if (dir != NULL) {
        struct dirent *ent;

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_CHR) {
                char absolute_path[PATH_MAX] = {0};
                char name[80] = {0};

                strcpy(absolute_path, "/dev/input/");
                strcat(absolute_path, ent->d_name);

                fd = open(absolute_path, O_RDWR);
                if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) > 0) {
                    if (strcmp(name, "fts_ts") == 0 ||
                            strcmp(name, "NVTCapacitiveTouchScreen") == 0)
                        break;
                }

                close(fd);
                fd = -1;
            }
        }

        closedir(dir);
    }

    return fd;
}

ndk::ScopedAStatus Power::setMode(Mode type, bool enabled) {
    LOG(VERBOSE) << "Power setMode: " << static_cast<int32_t>(type) << " to: " << enabled;

    switch (type) {
        case Mode::LAUNCH:
        {
            if (this->handle != 0) {
                libpowerhal_LockRel(this->handle);
                this->handle = 0;
            }

            if (enabled)
                this->handle = libpowerhal_CusLockHint(11, 30000, getpid());

            break;
        }
        case Mode::INTERACTIVE:
        {
            if (enabled)
                /* Device now in interactive state,
                   restore all currently held hints. */
                libpowerhal_UserScnRestoreAll();
            else
                /* Device entering non interactive state,
                   disable all hints to save power. */
                libpowerhal_UserScnDisableAll();
            break;
        }
        case Mode::DOUBLE_TAP_TO_WAKE:
        {
            int fd = open_ts_input();
            if (fd == -1)
                return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);

            struct input_event ev = {
                .type = EV_SYN,
                .code = SYN_CONFIG,
                .value = (enabled ? kWakeupModeOn : kWakeupModeOff),
             };
             write(fd, &ev, sizeof(ev));
             close(fd);
             break;
        }
        default:
            break;
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::isModeSupported(Mode type, bool* _aidl_return) {
    LOG(INFO) << "Power isModeSupported: " << static_cast<int32_t>(type);

    if (type == Mode::DOUBLE_TAP_TO_WAKE) {
        int fd = open_ts_input();
        if (fd != -1) {
            *_aidl_return = true;
            close(fd);
        }
    }
    else {
        *_aidl_return = type >= MODE_RANGE.front() && type <= MODE_RANGE.back();
    }

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::setBoost(Boost type, int32_t durationMs) {
    int32_t intType = static_cast<int32_t>(type);

    // Avoid boosts with 0 duration, as those will run indefinitely
    if (type == Boost::INTERACTION && durationMs < 1)
        durationMs = 80;

    LOG(VERBOSE) << "Power setBoost: " << intType
                 << ", duration: " << durationMs;

    libpowerhal_CusLockHint(intType, durationMs, getpid());
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::isBoostSupported(Boost type, bool* _aidl_return) {
    LOG(INFO) << "Power isBoostSupported: " << static_cast<int32_t>(type);
    *_aidl_return = type >= BOOST_RANGE.front() && type <= BOOST_RANGE.back();

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::createHintSession(int32_t, int32_t, const std::vector<int32_t>&, int64_t,
                                            std::shared_ptr<IPowerHintSession>* _aidl_return) {
    *_aidl_return = nullptr;
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Power::getHintSessionPreferredRate(int64_t* outNanoseconds) {
    *outNanoseconds = -1;
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace mediatek
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
