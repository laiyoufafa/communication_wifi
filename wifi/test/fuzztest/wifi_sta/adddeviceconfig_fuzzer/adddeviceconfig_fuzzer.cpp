/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "adddeviceconfig_fuzzer.h"
#include "wifi_device.h"

namespace OHOS {
namespace Wifi {
    bool AddDeviceConfigFuzzerTest(const uint8_t* data, size_t size)
    {
        std::unique_ptr<WifiDevice> devicePtr = WifiDevice::GetInstance(WIFI_DEVICE_ABILITY_ID);
        if (devicePtr == nullptr) {
            return false;
        }

        WifiDeviceConfig config;
        int addResult;
        config.ssid = std::string(reinterpret_cast<const char*>(data), size);
        config.bssid = std::string(reinterpret_cast<const char*>(data), size);
        config.preSharedKey = std::string(reinterpret_cast<const char*>(data), size);
        config.keyMgmt = std::string(reinterpret_cast<const char*>(data), size);
        devicePtr->AddDeviceConfig(config, addResult);
        return true;
    }
}  // namespace Wifi
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Wifi::AddDeviceConfigFuzzerTest(data, size);
    return 0;
}
