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
#ifndef OHOS_STA_MONITOR_H
#define OHOS_STA_MONITOR_H

#include "wifi_supplicant_hal_interface.h"
#include "sta_state_machine.h"

namespace OHOS {
namespace Wifi {
class StaMonitor {
public:
    /**
     * @Description : Construct a new Sta Monitor object.
     *
     */
    StaMonitor();

    /**
     * @Description Destroy the Sta Monitor object.
     *
     */
    ~StaMonitor();

    /**
     * @Description : Initialize the sta monitor.
     *
     */
    ErrCode InitStaMonitor();

    /**
     * @Description : Uninitialize the sta monitor.
     *
     */
    ErrCode UnInitStaMonitor() const;

    /**
     * @Description : Generating a Sta State Machine Instance.
     *
     * @param paraStaStateMachine - a Sta State Machine Instance [in]
     */
    void SetStateMachine(StaStateMachine *paraStaStateMachine);

    /**
     * @Description : Callback of the connection state change event.
     *
     * @param status : status codes [in]
     * @param networkId - network id [in]
     * @param bssid - bssid of the network [in]
     * @param pInstance - handles [in]
     */
    static void OnConnectChangedCallBack(int status, int networkId, char *bssid, void *pInstance);

    /**
     * @Description : Callback of the wpa state change event.
     *
     * @param status - status codes [in]
     * @param pInstance - handles [in]
     */
    static void OnWpaStateChangedCallBack(int status, void *pInstance);

    /**
     * @Description : Callback of the Wpa ssid wrong key event.
     *
     * @param status - status codes [in]
     * @param pInstance - handles [in]
     */
    static void OnWpaSsidWrongKeyCallBack(int status, void *pInstance);

    /**
     * @Description : Callback of the WPS_OVERLAP event.
     *
     * @param status - status codes [in]
     * @param pInstance - handles [in]
     */
    static void OnWpsPbcOverlapCallBack(int status, void *pInstance);

    /**
     * @Description : Callback of the WPS_TIMEOUT event.
     *
     * @param status - status codes [in]
     * @param pInstance - handles [in]
     */
    static void OnWPsTimeOutCallBack(int status, void *pInstance);

private:
    StaStateMachine *pStaStateMachine;
};
}  // namespace Wifi
}  // namespace OHOS
#endif