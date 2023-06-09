/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFISCANRESULT_H
#define OHOS_WIFISCANRESULT_H

#include <string>

namespace OHOS {
namespace Wifi {
class WifiScanResult {
public:
    std::string ssid;
    std::string bssid;
    std::string infoElement;
    int frequency;
    int signalLevel;
    int64_t timestamp;
    std::string capability;
    bool associated;
};
}  // namespace Wifi
}  // namespace OHOS

#endif