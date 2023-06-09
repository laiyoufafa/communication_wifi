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

#ifndef OHOS_WIFI_CONFIG_FILE_SPEC_H
#define OHOS_WIFI_CONFIG_FILE_SPEC_H
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "wifi_internal_msg.h"

namespace OHOS {
namespace Wifi {
/* ----------------- template function begin ----------------------- */
/**
 * @Description Clear and init item
 *
 * @tparam T - typename
 * @param item - item
 */
template <typename T>
void ClearTClass(T &item)
{
    /* fixed compile warning, -Werror,-Wunused-parameter */
    item;
    return;
}

/**
 * @Description Set item's data, input key is the item's member and input value is the
 *              member's value
 *
 * @tparam T - typename
 * @param item - T &item
 * @param key - Item key
 * @param value - Item value
 */
template <typename T>
void SetTClassKeyValue(T &item, const std::string &key, const std::string &value)
{
    /* fixed compile warning, -Werror,-Wunused-parameter */
    item;
    std::ostringstream ss;
    ss << key << value << std::endl;
    return;
}

/**
 * @Description Output the item's head
 *
 * @tparam T - typename
 * @param item - item
 * @return std::string - the item's type name
 */
template <typename T>
std::string GetTClassName()
{
    return "";
}

/**
 * @Description Output the item, format: item's member = the member value
 *
 * @tparam T - typename
 * @param item - item
 * @return std::string - output item's total member=value string
 */
template <typename T>
std::string OutTClassString(T &item)
{
    /* fixed compile warning, -Werror,-Wunused-parameter */
    item;
    std::string s;
    return s;
}

/* ----------------- template function end --------------------------------- */

/* ------------template function specialization declare begin-------------- */
/**
 * @Description Clear and init WifiDeviceConfig
 *
 * @tparam
 * @param item - WifiDeviceConfig item
 */
template <>
void ClearTClass<WifiDeviceConfig>(WifiDeviceConfig &item);

/**
 * @Description Set WifiDeviceConfig item data
 *
 * @tparam
 * @param item - WifiDeviceConfig &item
 * @param key - WifiDeviceConfig struct member name
 * @param value - the WifiDeviceConfig item member value
 */
template <>
void SetTClassKeyValue<WifiDeviceConfig>(WifiDeviceConfig &item, const std::string &key, const std::string &value);

/**
 * @Description Output WifiDeviceConfig class name
 *
 * @tparam
 * @param item - WifiDeviceConfig &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<WifiDeviceConfig>();

/**
 * @Description Output the WifiDeviceConfig item, format: item's member = the member value
 *
 * @tparam
 * @param item - WifiDeviceConfig &item
 * @return std::string - output total member=value string about the WifiDeviceConfig item
 */
template <>
std::string OutTClassString<WifiDeviceConfig>(WifiDeviceConfig &item);

/**
 * @Description Clear and init HotspotConfig
 *
 * @tparam
 * @param item - HotspotConfig item
 */
template <>
void ClearTClass<HotspotConfig>(HotspotConfig &item);

/**
 * @Description Set HotspotConfig item data
 *
 * @tparam
 * @param item - HotspotConfig &item
 * @param key - HotspotConfig struct member name
 * @param value - the HotspotConfig item member value
 */
template <>
void SetTClassKeyValue<HotspotConfig>(HotspotConfig &item, const std::string &key, const std::string &value);

/**
 * @Description Output HotspotConfig class name
 *
 * @tparam
 * @param item - HotspotConfig &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<HotspotConfig>();

/**
 * @Description Output the HotspotConfig item, format: item's member = the member value
 *
 * @tparam
 * @param item - HotspotConfig &item
 * @return std::string - output total member=value string about the HotspotConfig item
 */
template <>
std::string OutTClassString<HotspotConfig>(HotspotConfig &item);

/**
 * @Description Clear and init StationInfo
 *
 * @tparam
 * @param item - StationInfo &item
 */
template <>
void ClearTClass<StationInfo>(StationInfo &item);

/**
 * @Description Set StationInfo item data
 *
 * @tparam
 * @param item - StationInfo &item
 * @param key - StationInfo struct member name
 * @param value - the StationInfo item member value
 */
template <>
void SetTClassKeyValue<StationInfo>(StationInfo &item, const std::string &key, const std::string &value);

/**
 * @Description Output StationInfo class name
 *
 * @tparam
 * @param item - StationInfo &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<StationInfo>();

/**
 * @Description Output the StationInfo item, format: item's member = the member value
 *
 * @tparam
 * @param item - StationInfo &item
 * @return std::string - output total member=value string about the StationInfo item
 */
template <>
std::string OutTClassString<StationInfo>(StationInfo &item);

/**
 * @Description Clear and init WifiConfig
 *
 * @tparam
 * @param item - WifiConfig &item
 */
template <>
void ClearTClass<WifiConfig>(WifiConfig &item);

/**
 * @Description Set WifiConfig item data
 *
 * @tparam
 * @param item - WifiConfig &item
 * @param key - WifiConfig struct member name
 * @param value - the WifiConfig item member value
 */
template <>
void SetTClassKeyValue<WifiConfig>(WifiConfig &item, const std::string &key, const std::string &value);

/**
 * @Description Output WifiConfig class name
 *
 * @tparam
 * @param item - WifiConfig &item
 * @return std::string - Class name
 */
template <>
std::string GetTClassName<WifiConfig>();

/**
 * @Description Output the WifiConfig item, format: item's member = the member value
 *
 * @tparam
 * @param item - WifiConfig &item
 * @return std::string - output total member=value string about the WifiConfig item
 */
template <>
std::string OutTClassString<WifiConfig>(WifiConfig &item);
/* ----------template function specialization declare end----------- */
}  // namespace Wifi
}  // namespace OHOS
#endif