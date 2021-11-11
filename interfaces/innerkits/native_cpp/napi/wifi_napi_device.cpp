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

#include "wifi_napi_device.h"
#include "wifi_logger.h"
#include <vector>
#include <functional>

namespace OHOS {
namespace Wifi {
DEFINE_WIFILOG_LABEL("WifiNAPIDevice");

std::unique_ptr<WifiDevice> wifiDevicePtr = WifiDevice::GetInstance(WIFI_DEVICE_ABILITY_ID);
std::unique_ptr<WifiScan> wifiScanPtr = WifiScan::GetInstance(WIFI_SCAN_ABILITY_ID);
napi_value EnableWifi(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    ErrCode ret = wifiDevicePtr->EnableWifi();
    napi_value result;
    napi_get_boolean(env, ret == WIFI_OPT_SUCCESS, &result);
    return result;
}

napi_value DisableWifi(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    ErrCode ret = wifiDevicePtr->DisableWifi();
    napi_value result;
    napi_get_boolean(env, ret == WIFI_OPT_SUCCESS, &result);
    return result;
}

napi_value IsWifiActive(napi_env env, napi_callback_info info)
{
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    bool activeStatus = false;
    ErrCode ret = wifiDevicePtr->IsWifiActive(activeStatus);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Get wifi active status fail: %{public}d", ret);
    }

    napi_value result;
    napi_get_boolean(env, activeStatus, &result);
    return result;
}

napi_value Scan(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiScanPtr != nullptr, "Wifi scan instance is null.");
    ErrCode ret = wifiScanPtr->Scan();

    napi_value result;
    napi_get_boolean(env, ret == WIFI_OPT_SUCCESS, &result);
    return result;
}

static SecTypeJs SecurityTypeNativeToJs(const WifiSecurity& cppSecurityType)
{
    SecTypeJs jsSecurityType = SecTypeJs::SEC_TYPE_INVALID;
    switch (cppSecurityType) {
        case WifiSecurity::OPEN:
            jsSecurityType = SecTypeJs::SEC_TYPE_OPEN;
            break;

        case WifiSecurity::WEP:
            jsSecurityType = SecTypeJs::SEC_TYPE_WEP;
            break;

        case WifiSecurity::PSK:
            jsSecurityType = SecTypeJs::SEC_TYPE_PSK;
            break;

        case WifiSecurity::SAE:
            jsSecurityType = SecTypeJs::SEC_TYPE_SAE;
            break;

        default:
            jsSecurityType = SecTypeJs::SEC_TYPE_INVALID;
            break;
    }
    return jsSecurityType;
}

static bool NativeScanInfosToJsObj(const napi_env& env,
    const std::vector<WifiScanInfo>& vecScnIanfos, napi_value& arrayResult)
{
    uint32_t idx = 0;
    for (auto& each : vecScnIanfos) {
        napi_value eachObj;
        napi_create_object(env, &eachObj);

        SetValueUtf8String(env, "ssid", each.ssid.c_str(), eachObj);
        SetValueUtf8String(env, "bssid", each.bssid.c_str(), eachObj);
        SetValueInt32(env, "securityType", static_cast<int>(SecurityTypeNativeToJs(each.securityType)), eachObj);
        SetValueInt32(env, "rssi", each.rssi, eachObj);
        SetValueInt32(env, "band", each.band, eachObj);
        SetValueInt32(env, "frequency", each.frequency, eachObj);
        SetValueInt64(env, "timestamp", each.timestamp, eachObj);

        napi_status status = napi_set_element(env, arrayResult, idx++, eachObj);
        if (status != napi_ok) {
            WIFI_LOGE("Wifi napi set element error: %{public}d, idx: %{public}d", status, idx - 1);
            return false;
        }
    }
    return true;
}

napi_value GetScanInfos(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, wifiScanPtr != nullptr, "Wifi device instance is null.");

    ScanInfoAsyncContext *asyncContext = new ScanInfoAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    napi_create_string_latin1(env, "getScanInfos", NAPI_AUTO_LENGTH, &asyncContext->resourceName);

    asyncContext->executeFunc = [&](void* data) -> void {
        ScanInfoAsyncContext *context = static_cast<ScanInfoAsyncContext *>(data);
        TRACE_FUNC_CALL_NAME("wifiScanPtr->GetScanInfoList");
        context->isSuccess = (wifiScanPtr->GetScanInfoList(context->vecScanInfos) == WIFI_OPT_SUCCESS);
        WIFI_LOGI("GetScanInfoList, size: %{public}zu", context->vecScanInfos.size());
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        ScanInfoAsyncContext *context = static_cast<ScanInfoAsyncContext *>(data);
        napi_create_array_with_length(context->env, context->vecScanInfos.size(), &context->result);
        context->isSuccess = NativeScanInfosToJsObj(context->env, context->vecScanInfos, context->result);
        WIFI_LOGI("Push scan info list to client");
    };

    size_t nonCallbackArgNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, nonCallbackArgNum);
}

static void ConvertEncryptionMode(const SecTypeJs& securityType, std::string& keyMgmt)
{
    switch (securityType) {
        case SecTypeJs::SEC_TYPE_OPEN:
            keyMgmt = "NONE";
            break;

        case SecTypeJs::SEC_TYPE_WEP:
            keyMgmt = "WEP";
            break;

        case SecTypeJs::SEC_TYPE_PSK:
            keyMgmt = "WPA-PSK";
            break;

        case SecTypeJs::SEC_TYPE_SAE:
            keyMgmt = "SAE";
            break;

        default:
            keyMgmt = "NONE";
            break;
    }
}

static void JsObjToDeviceConfig(const napi_env& env, const napi_value& object, WifiDeviceConfig& cppConfig)
{
    JsObjectToString(env, object, "ssid", 33, cppConfig.ssid); /* 33: ssid max length is 32 + '\0' */
    JsObjectToString(env, object, "bssid", 18, cppConfig.bssid); /* 18: max bssid length for string type */
    JsObjectToString(env, object, "preSharedKey", 256, cppConfig.preSharedKey); /* 256: max length */
    JsObjectToBool(env, object, "isHiddenSsid", cppConfig.hiddenSSID);
    int type = static_cast<int>(SecTypeJs::SEC_TYPE_INVALID);
    JsObjectToInt(env, object, "securityType", type);
    ConvertEncryptionMode(SecTypeJs(type), cppConfig.keyMgmt);
}

napi_value AddDeviceConfig(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 3;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    
    napi_valuetype valueType;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type, object is expected for parameter 1.");

    AddDeviceConfigContext *asyncContext = new AddDeviceConfigContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    napi_create_string_latin1(env, "addDeviceConfig", NAPI_AUTO_LENGTH, &asyncContext->resourceName);

    WifiDeviceConfig *config = new WifiDeviceConfig();
    if (config == nullptr) {
        delete asyncContext;
        return UndefinedNapiValue(env);
    }
    JsObjToDeviceConfig(env, argv[0], *config);
    asyncContext->config = config;

    asyncContext->executeFunc = [&](void* data) -> void {
        AddDeviceConfigContext *context = static_cast<AddDeviceConfigContext *>(data);
        TRACE_FUNC_CALL_NAME("wifiDevicePtr->AddDeviceConfig");
        ErrCode ret = wifiDevicePtr->AddDeviceConfig(*context->config, context->addResult);
        if (context->addResult < 0 || ret != WIFI_OPT_SUCCESS) {
            context->addResult = -1;
        }
        context->isSuccess = (ret == WIFI_OPT_SUCCESS);
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        AddDeviceConfigContext *context = static_cast<AddDeviceConfigContext *>(data);
        napi_create_int32(context->env, context->addResult, &context->result);
        if (context->config != nullptr) {
            delete context->config;
            context->config = nullptr;
        }
        WIFI_LOGI("Push add device config result to client");
    };

    size_t nonCallbackArgNum = 1;
    return DoAsyncWork(env, asyncContext, argc, argv, nonCallbackArgNum);
}

napi_value ConnectToNetwork(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype valueType;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_number, "Wrong argument type. napi_number expected.");

    int networkId = -1;
    napi_get_value_int32(env, argv[0], &networkId);

    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    ErrCode ret = wifiDevicePtr->ConnectToNetwork(networkId);
    napi_value result;
    napi_get_boolean(env, ret == WIFI_OPT_SUCCESS, &result);
    return result;
}

napi_value ConnectToDevice(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));

    napi_valuetype valueType;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type. Object expected.");

    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    WifiDeviceConfig config;
    JsObjToDeviceConfig(env, argv[0], config);
    ErrCode ret = wifiDevicePtr->ConnectToDevice(config);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Connect to device fail: %{public}d", ret);
    }

    napi_value result;
    napi_get_boolean(env, ret == WIFI_OPT_SUCCESS, &result);
    return result;
}

napi_value Disconnect(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    ErrCode ret = wifiDevicePtr->Disconnect();
    napi_value result;
    napi_get_boolean(env, ret == WIFI_OPT_SUCCESS, &result);
    return result;
}

napi_value GetSignalLevel(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    /* the input have 2 parameters */
    NAPI_ASSERT(env, argc == 2, "Wrong number of arguments");

    napi_valuetype type1;
    napi_valuetype type2;
    napi_typeof(env, argv[0], &type1);
    napi_typeof(env, argv[1], &type2);
    NAPI_ASSERT(env, type1 == napi_number, "Wrong argument type. napi_number expected.");
    NAPI_ASSERT(env, type2 == napi_number, "Wrong argument type. napi_number expected.");
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");

    int level = -1;
    int rssi = 0;
    int band = 0;
    napi_get_value_int32(env, argv[0], &rssi);
    napi_get_value_int32(env, argv[1], &band);
    ErrCode ret = wifiDevicePtr->GetSignalLevel(rssi, band, level);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Get wifi signal level fail: %{public}d", ret);
    }

    napi_value result;
    napi_create_uint32(env, level, &result);
    return result;
}

/* This interface has not been implemented */
napi_value ReConnect(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    napi_value result = nullptr;
    return result;
}

/* This interface has not been implemented */
napi_value ReAssociate(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    napi_value result = nullptr;
    return result;
}

/* This interface has not been implemented */
napi_value GetIpInfo(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    napi_value result = nullptr;
    return result;
}

static void LinkedInfoToJs(const napi_env& env, WifiLinkedInfo& linkedInfo, napi_value& result)
{
    SetValueUtf8String(env, "ssid", linkedInfo.ssid.c_str(), result);
    SetValueUtf8String(env, "bssid", linkedInfo.bssid.c_str(), result);
    SetValueInt32(env, "networkId", linkedInfo.networkId, result);
    SetValueInt32(env, "rssi", linkedInfo.rssi, result);
    SetValueInt32(env, "band", linkedInfo.band, result);
    SetValueInt32(env, "linkSpeed", linkedInfo.linkSpeed, result);
    SetValueInt32(env, "frequency", linkedInfo.frequency, result);
    SetValueBool(env, "isHidden", linkedInfo.ifHiddenSSID, result);
    /* isRestricted not support now, set as default value */
    SetValueBool(env, "isRestricted", false, result);
    SetValueInt32(env, "chload", linkedInfo.chload, result);
    SetValueInt32(env, "snr", linkedInfo.snr, result);
    SetValueUtf8String(env, "macAddress", linkedInfo.macAddress.c_str(), result);
    SetValueInt32(env, "ipAddress", linkedInfo.ipAddress, result);
    /* Check suppState is consistent with HOS */
    SetValueInt32(env, "suppState", static_cast<int>(linkedInfo.supplicantState), result);
    /* Check connState is consistent with HOS */
    SetValueInt32(env, "connState", static_cast<int>(linkedInfo.connState), result);
}

/* This interface has not been fully implemented */
napi_value GetLinkedInfo(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");

    LinkedInfoAsyncContext *asyncContext = new LinkedInfoAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    napi_create_string_latin1(env, "getLinkedInfo", NAPI_AUTO_LENGTH, &asyncContext->resourceName);

    asyncContext->executeFunc = [&](void* data) -> void {
        LinkedInfoAsyncContext *context = static_cast<LinkedInfoAsyncContext *>(data);
        TRACE_FUNC_CALL_NAME("wifiDevicePtr->GetLinkedInfo");
        context->isSuccess = (wifiDevicePtr->GetLinkedInfo(context->linkedInfo) == WIFI_OPT_SUCCESS);
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        LinkedInfoAsyncContext *context = static_cast<LinkedInfoAsyncContext *>(data);
        napi_create_object(context->env, &context->result);
        LinkedInfoToJs(context->env, context->linkedInfo, context->result);
        WIFI_LOGI("Push get linkedInfo result to client");
    };

    size_t nonCallbackArgNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, nonCallbackArgNum);
}

napi_value RemoveDevice(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype valueType;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_number, "Wrong argument type. napi_number expected.");

    int networkId = -1;
    napi_get_value_int32(env, argv[0], &networkId);
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");

    napi_value result;
    napi_get_boolean(env, wifiDevicePtr->RemoveDevice(networkId) == WIFI_OPT_SUCCESS, &result);
    return result;
}

napi_value RemoveAllNetwork(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    napi_value result;
    napi_get_boolean(env, wifiDevicePtr->RemoveAllDevice() == WIFI_OPT_SUCCESS, &result);
    return result;
}

/* This interface has not been implemented */
napi_value DisableNetwork(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype valueType;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_number, "Wrong argument type. napi_number expected.");
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");

    int networkId = -1;
    napi_get_value_int32(env, argv[0], &networkId);
    napi_value result = nullptr;
    return result;
}

napi_value GetCountryCode(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    std::string countryCode;
    ErrCode ret = wifiDevicePtr->GetCountryCode(countryCode);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Get countryCode fail: %{public}d", ret);
    }
    napi_value cc;
    napi_create_string_utf8(env, countryCode.c_str(), NAPI_AUTO_LENGTH, &cc);
    return cc;
}

static SecTypeJs ConvertKeyMgmtToSecType(const std::string& keyMgmt)
{
    std::map<std::string, SecTypeJs> mapKeyMgmtToSecType = {
        {"NONE", SecTypeJs::SEC_TYPE_OPEN},
        {"WEP", SecTypeJs::SEC_TYPE_WEP},
        {"WPA-PSK", SecTypeJs::SEC_TYPE_PSK},
        {"SAE", SecTypeJs::SEC_TYPE_SAE},
    };

    std::map<std::string, SecTypeJs>::iterator iter = mapKeyMgmtToSecType.find(keyMgmt);
    return iter == mapKeyMgmtToSecType.end() ? SecTypeJs::SEC_TYPE_OPEN : iter->second;
}

static void DeviceConfigToJsArray(const napi_env& env, const std::vector<WifiDeviceConfig>& vecDeviceConfigs,
    const int idx, napi_value& arrayResult)
{
    napi_value result;
    napi_create_object(env, &result);
    SetValueUtf8String(env, "ssid", vecDeviceConfigs[idx].ssid.c_str(), result);
    SetValueUtf8String(env, "bssid", vecDeviceConfigs[idx].bssid.c_str(), result);
    SetValueUtf8String(env, "preSharedKey", vecDeviceConfigs[idx].preSharedKey.c_str(), result);
    SetValueBool(env, "isHiddenSsid", vecDeviceConfigs[idx].hiddenSSID, result);
    SetValueInt32(env, "securityType",
        static_cast<int>(ConvertKeyMgmtToSecType(vecDeviceConfigs[idx].keyMgmt)), result);

    napi_status status = napi_set_element(env, arrayResult, idx, result);
    if (status != napi_ok) {
        WIFI_LOGE("Wifi napi set element error: %{public}d", status);
    }
}

napi_value GetDeviceConfigs(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    std::vector<WifiDeviceConfig> vecDeviceConfigs;
    ErrCode ret = wifiDevicePtr->GetDeviceConfigs(vecDeviceConfigs);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Get device configs fail: %{public}d", ret);
    }

    WIFI_LOGI("Get device configs size: %{public}zu", vecDeviceConfigs.size());
    napi_value arrayResult;
    napi_create_array_with_length(env, vecDeviceConfigs.size(), &arrayResult);
    for (size_t i = 0; i != vecDeviceConfigs.size(); ++i) {
        DeviceConfigToJsArray(env, vecDeviceConfigs, i, arrayResult);
    }
    return arrayResult;
}

napi_value GetSupportedFeatures(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    long features = -1;
    ErrCode ret = wifiDevicePtr->GetSupportedFeatures(features);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Get supported features fail: %{public}d", ret);
    }

    napi_value result;
    napi_create_int64(env, features, &result);
    return result;
}

napi_value IsFeatureSupported(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype valueType;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_number, "Wrong argument type. napi_number expected.");

    long feature = -1;
    napi_get_value_int64(env, argv[0], (int64_t*)&feature);
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");

    napi_value result;
    napi_get_boolean(env, wifiDevicePtr->IsFeatureSupported(feature), &result);
    return result;
}

napi_value GetDeviceMacAddress(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    NAPI_ASSERT(env, wifiDevicePtr != nullptr, "Wifi device instance is null.");
    std::string macAddr;
    ErrCode ret = wifiDevicePtr->GetDeviceMacAddress(macAddr);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Get mac address fail: %{public}d", ret);
    }

    napi_value addr;
    napi_create_string_utf8(env, macAddr.c_str(), NAPI_AUTO_LENGTH, &addr);
    return addr;
}
}  // namespace Wifi
}  // namespace OHOS
