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

#include "wifi_hotspot_callback_stub.h"
#include "wifi_logger.h"
#include "wifi_msg.h"
#include "define.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiHotspotCallbackStub");
namespace OHOS {
namespace Wifi {
WifiHotspotCallbackStub::WifiHotspotCallbackStub() : userCallback_(nullptr), mRemoteDied(false)
{}

WifiHotspotCallbackStub::~WifiHotspotCallbackStub()
{}

int WifiHotspotCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("WifiHotspotCallbackStub::OnRemoteRequest!");
    if (mRemoteDied) {
        WIFI_LOGD("Failed to `%{public}s`,remote service is died!", __func__);
        return -1;
    }
    int exception = data.ReadInt32();
    if (exception) {
        return -1;
    }
    int ret = -1;
    switch (code) {
        case WIFI_CBK_CMD_HOTSPOT_STATE_CHANGE: {
            ret = RemoteOnHotspotStateChanged(code, data, reply);
            break;
        }
        case WIFI_CBK_CMD_HOTSPOT_STATE_JOIN: {
            ret = RemoteOnHotspotStaJoin(code, data, reply);
            break;
        }
        case WIFI_CBK_CMD_HOTSPOT_STATE_LEAVE: {
            ret = RemoteOnHotspotStaLeave(code, data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

int WifiHotspotCallbackStub::RemoteOnHotspotStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int state = data.ReadInt32();
    OnHotspotStateChanged(state);
    reply.WriteInt32(0);
    return 0;
}

int WifiHotspotCallbackStub::RemoteOnHotspotStaJoin(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    StationInfo info;
    info.deviceName = Str16ToStr8(data.ReadString16());
    info.bssid = Str16ToStr8(data.ReadString16());
    info.ipAddr = Str16ToStr8(data.ReadString16());
    OnHotspotStaJoin(info);
    reply.WriteInt32(0);
    return 0;
}

int WifiHotspotCallbackStub::RemoteOnHotspotStaLeave(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    StationInfo info;
    info.deviceName = Str16ToStr8(data.ReadString16());
    info.bssid = Str16ToStr8(data.ReadString16());
    info.ipAddr = Str16ToStr8(data.ReadString16());
    OnHotspotStaLeave(info);
    reply.WriteInt32(0);
    return 0;
}

void WifiHotspotCallbackStub::RegisterCallBack(const sptr<IWifiHotspotCallback> &callBack)
{
    if (callBack == nullptr) {
        WIFI_LOGD("RegisterCallBack:callBack is nullptr!");
    }
    userCallback_ = callBack;
    return;
}

void WifiHotspotCallbackStub::OnHotspotStateChanged(int state)
{
    WIFI_LOGD("WifiHotspotCallbackStub::OnHotspotStateChanged");
    if (userCallback_) {
        userCallback_->OnHotspotStateChanged(state);
    }
}

void WifiHotspotCallbackStub::OnHotspotStaJoin(const StationInfo &info)
{
    WIFI_LOGD("WifiHotspotCallbackStub::OnHotspotStaJoin");
    if (userCallback_) {
        userCallback_->OnHotspotStaJoin(info);
    }
}

void WifiHotspotCallbackStub::OnHotspotStaLeave(const StationInfo &info)
{
    WIFI_LOGD("WifiHotspotCallbackStub::OnHotspotStaLeave");
    if (userCallback_) {
        userCallback_->OnHotspotStaLeave(info);
    }
}

bool WifiHotspotCallbackStub::IsRemoteDied() const
{
    return mRemoteDied;
}

void WifiHotspotCallbackStub::SetRemoteDied(bool val)
{
    mRemoteDied = val;
}
}  // namespace Wifi
}  // namespace OHOS