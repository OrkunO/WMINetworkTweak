#include "WiFiUtil.h";

WiFiUtil::WiFiUtil()
{
    hClient = NULL;
    pIfInfo = NULL;
    pIfList = NULL;
    pBssList = NULL;
    pBssEntry = NULL;
    dwRetVal = 0;
    dwResult = 0;
    dwMaxClient = 2;
    dwCurVersion = 0;
}

WiFiUtil::~WiFiUtil()
{
    if (pBssList != NULL) {
        WlanFreeMemory(pBssList);
        pBssList = NULL;
    }

    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }
}

bool WiFiUtil::initialize() {
    PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

    int iRSSI = 0;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return false;
        // You can use FormatMessage here to find out why the function failed
    }
    return true;
}

bool WiFiUtil::connectToWiFi(string wifiSSID) {
    PWLAN_AVAILABLE_NETWORK foundNetwork = NULL;
    unsigned int i, j;
    for (i = 0; i < pBssList->dwNumberOfItems; i++) {
        pBssEntry =
            (WLAN_AVAILABLE_NETWORK*)&pBssList->Network[j];

        wprintf(L"  Profile Name[%u]:  %ws\n", i, pBssEntry->strProfileName);

        wprintf(L"  SSID[%u]:\t\t ", j);
        if (pBssEntry->dot11Ssid.uSSIDLength == 0)
            wprintf(L"\n");
        else {
            std::string strDot11Ssid = "";
            for (j = 0; j < pBssEntry->dot11Ssid.uSSIDLength; j++) {
                wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[j]);
                strDot11Ssid += pBssEntry->dot11Ssid.ucSSID[j];
            }
            if (strDot11Ssid == wifiSSID)
            {
                foundNetwork = pBssEntry;
                break;
            }
        }
    }
    if (foundNetwork != NULL)
    {
        OSVERSIONINFO osvi;
        memset(&osvi, 0, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        DOT11_BSSID_LIST DesiredBssidList;
        WLAN_CONNECTION_PARAMETERS* wlanParam = (WLAN_CONNECTION_PARAMETERS*)WlanAllocateMemory(sizeof(WLAN_CONNECTION_PARAMETERS));
        GetVersionEx(&osvi);
        std::cout << "OS DW MAJOR VERSION: " << osvi.dwMajorVersion << std::endl;
        if (osvi.dwMajorVersion >= 6) {
            WLAN_BSS_LIST* bsslst;
            WlanGetNetworkBssList(hClient, &pIfInfo->InterfaceGuid, &foundNetwork->dot11Ssid, foundNetwork->dot11BssType, true, NULL, &bsslst);
            DesiredBssidList.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
            DesiredBssidList.Header.Revision = DOT11_BSSID_LIST_REVISION_1;
            DesiredBssidList.Header.Size = sizeof(DOT11_BSSID_LIST);
            DesiredBssidList.uNumOfEntries = 1; // If I change this to 0, it connects without a problem, but I can't control to which AP
            DesiredBssidList.uTotalNumOfEntries = 1;
            DesiredBssidList.BSSIDs[0][0] = bsslst->wlanBssEntries->dot11Bssid[0];
            DesiredBssidList.BSSIDs[0][1] = bsslst->wlanBssEntries->dot11Bssid[1];
            DesiredBssidList.BSSIDs[0][2] = bsslst->wlanBssEntries->dot11Bssid[2];
            DesiredBssidList.BSSIDs[0][3] = bsslst->wlanBssEntries->dot11Bssid[3];
            DesiredBssidList.BSSIDs[0][4] = bsslst->wlanBssEntries->dot11Bssid[4];
            DesiredBssidList.BSSIDs[0][5] = bsslst->wlanBssEntries->dot11Bssid[5]; wlanParam->pDesiredBssidList = &DesiredBssidList;
        }
        else {
            wlanParam->pDesiredBssidList = NULL;
        }

        WLAN_REASON_CODE wlanReasonCode;
        DWORD dwResult = 0;
        DWORD dwPrevNotif = 0;
        LPCWSTR apName = LPCWSTR("some SSID");
        wlanParam->wlanConnectionMode = wlan_connection_mode_profile;
        wlanParam->dot11BssType = foundNetwork->dot11BssType;
        wlanParam->dwFlags = 0;
        std::cout << "before foundD11SSID.dot11Ssid" << std::endl;
        wlanParam->pDot11Ssid = &foundNetwork->dot11Ssid;
        printf("before ConvertToTchar\n");
        wstring wstrProfile = StringHelper::s2ws(wifiSSID);
        LPCWSTR strProfile = wstrProfile.c_str();
        wlanParam->strProfile = strProfile;
        wprintf(L"%s\n", wlanParam->strProfile);

        dwResult = WlanConnect(hClient, &pIfInfo->InterfaceGuid, wlanParam, NULL);
        if (dwResult != ERROR_SUCCESS)
        {
            wprintf(L"WlanConnect failed with error: %u\n",
                dwResult);
            dwRetVal = 1;
            return false;
            // You can use FormatMessage to find out why the function failed
        }
        else {
            wprintf(L"Connected successfully\n");
            return true;
        }
    }
    return false;
}

const vector<WiFiInfo> WiFiUtil::getAvailableNetworkList(string interfaceGUID) {
    unsigned int i, j, k;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    int iRet = 0;
    int iRSSI = 0;

    WCHAR GuidString[39] = { 0 };

    vector<WiFiInfo> wifiInfos;
    wifiInfos.clear();

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        WiFiInfo wiFiInfo;
        wiFiInfo.ssid = "-1";
        wiFiInfo.flags = 0;
        wiFiInfo.iRRSI = 0;
        wifiInfos.push_back(wiFiInfo);
        return wifiInfos;
        // You can use FormatMessage here to find out why the function failed
    }
    else {
        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
            pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];
            iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
                sizeof(GuidString) / sizeof(*GuidString));
            std::wstring wstrGuid(GuidString);
            string strGuid = StringHelper::ws2s(wstrGuid);
            if (strGuid == interfaceGUID) {
                wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);
                iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
                    sizeof(GuidString) / sizeof(*GuidString));
                // For c rather than C++ source code, the above line needs to be
                // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
                //     sizeof(GuidString)/sizeof(*GuidString)); 
                if (iRet == 0)
                    wprintf(L"StringFromGUID2 failed\n");
                else {
                    wprintf(L"  InterfaceGUID[%d]: %ws\n", i, GuidString);
                }
                wprintf(L"  Interface Description[%d]: %ws", i,
                    pIfInfo->strInterfaceDescription);
                wprintf(L"\n");
                wprintf(L"  Interface State[%d]:\t ", i);
                switch (pIfInfo->isState) {
                case wlan_interface_state_not_ready:
                    wprintf(L"Not ready\n");
                    break;
                case wlan_interface_state_connected:
                    wprintf(L"Connected\n");
                    break;
                case wlan_interface_state_ad_hoc_network_formed:
                    wprintf(L"First node in a ad hoc network\n");
                    break;
                case wlan_interface_state_disconnecting:
                    wprintf(L"Disconnecting\n");
                    break;
                case wlan_interface_state_disconnected:
                    wprintf(L"Not connected\n");
                    break;
                case wlan_interface_state_associating:
                    wprintf(L"Attempting to associate with a network\n");
                    break;
                case wlan_interface_state_discovering:
                    wprintf(L"Auto configuration is discovering settings for the network\n");
                    break;
                case wlan_interface_state_authenticating:
                    wprintf(L"In process of authenticating\n");
                    break;
                default:
                    wprintf(L"Unknown state %ld\n", pIfInfo->isState);
                    break;
                }
                wprintf(L"\n");

                dwResult = WlanGetAvailableNetworkList(hClient,
                    &pIfInfo->InterfaceGuid,
                    0,
                    NULL,
                    &pBssList);

                if (dwResult != ERROR_SUCCESS) {
                    wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n",
                        dwResult);
                    dwRetVal = 1;
                    // You can use FormatMessage to find out why the function failed
                }
                else {
                    wprintf(L"WLAN_AVAILABLE_NETWORK_LIST for this interface\n");

                    wprintf(L"  Num Entries: %lu\n\n", pBssList->dwNumberOfItems);

                    for (j = 0; j < pBssList->dwNumberOfItems; j++) {
                        pBssEntry =
                            (WLAN_AVAILABLE_NETWORK*)&pBssList->Network[j];
                        WiFiInfo wiFiInfo;

                        wprintf(L"  Profile Name[%u]:  %ws\n", j, pBssEntry->strProfileName);

                        wprintf(L"  SSID[%u]:\t\t ", j);
                        if (pBssEntry->dot11Ssid.uSSIDLength == 0)
                            wprintf(L"\n");
                        else {
                            std::string strDot11Ssid = "";
                            for (k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
                                wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[k]);
                                strDot11Ssid += pBssEntry->dot11Ssid.ucSSID[k];
                            }
                            wiFiInfo.ssid = strDot11Ssid;
                            wprintf(L"\n");
                        }

                        wprintf(L"  BSS Network type[%u]:\t ", j);
                        switch (pBssEntry->dot11BssType) {
                        case dot11_BSS_type_infrastructure:
                            wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
                            break;
                        case dot11_BSS_type_independent:
                            wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
                            break;
                        default:
                            wprintf(L"Other (%lu)\n", pBssEntry->dot11BssType);
                            break;
                        }

                        wprintf(L"  Number of BSSIDs[%u]:\t %u\n", j, pBssEntry->uNumberOfBssids);

                        wprintf(L"  Connectable[%u]:\t ", j);
                        if (pBssEntry->bNetworkConnectable)
                            wprintf(L"Yes\n");
                        else {
                            wprintf(L"No\n");
                            wprintf(L"  Not connectable WLAN_REASON_CODE value[%u]:\t %u\n", j,
                                pBssEntry->wlanNotConnectableReason);
                        }

                        wprintf(L"  Number of PHY types supported[%u]:\t %u\n", j, pBssEntry->uNumberOfPhyTypes);

                        if (pBssEntry->wlanSignalQuality == 0)
                            iRSSI = -100;
                        else if (pBssEntry->wlanSignalQuality == 100)
                            iRSSI = -50;
                        else
                            iRSSI = -100 + (pBssEntry->wlanSignalQuality / 2);

                        wiFiInfo.iRRSI = iRSSI;

                        wprintf(L"  Signal Quality[%u]:\t %u (RSSI: %i dBm)\n", j,
                            pBssEntry->wlanSignalQuality, iRSSI);

                        wprintf(L"  Security Enabled[%u]:\t ", j);
                        if (pBssEntry->bSecurityEnabled)
                            wprintf(L"Yes\n");
                        else
                            wprintf(L"No\n");

                        wprintf(L"  Default AuthAlgorithm[%u]: ", j);
                        switch (pBssEntry->dot11DefaultAuthAlgorithm) {
                        case DOT11_AUTH_ALGO_80211_OPEN:
                            wprintf(L"802.11 Open (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
                            wprintf(L"802.11 Shared (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        case DOT11_AUTH_ALGO_WPA:
                            wprintf(L"WPA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        case DOT11_AUTH_ALGO_WPA_PSK:
                            wprintf(L"WPA-PSK (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        case DOT11_AUTH_ALGO_WPA_NONE:
                            wprintf(L"WPA-None (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        case DOT11_AUTH_ALGO_RSNA:
                            wprintf(L"RSNA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        case DOT11_AUTH_ALGO_RSNA_PSK:
                            wprintf(L"RSNA with PSK(%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        default:
                            wprintf(L"Other (%lu)\n", pBssEntry->dot11DefaultAuthAlgorithm);
                            break;
                        }

                        wprintf(L"  Default CipherAlgorithm[%u]: ", j);
                        switch (pBssEntry->dot11DefaultCipherAlgorithm) {
                        case DOT11_CIPHER_ALGO_NONE:
                            wprintf(L"None (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
                            break;
                        case DOT11_CIPHER_ALGO_WEP40:
                            wprintf(L"WEP-40 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
                            break;
                        case DOT11_CIPHER_ALGO_TKIP:
                            wprintf(L"TKIP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
                            break;
                        case DOT11_CIPHER_ALGO_CCMP:
                            wprintf(L"CCMP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
                            break;
                        case DOT11_CIPHER_ALGO_WEP104:
                            wprintf(L"WEP-104 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
                            break;
                        case DOT11_CIPHER_ALGO_WEP:
                            wprintf(L"WEP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
                            break;
                        default:
                            wprintf(L"Other (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
                            break;
                        }

                        wprintf(L"  Flags[%u]:\t 0x%x", j, pBssEntry->dwFlags);
                        wiFiInfo.flags = pBssEntry->dwFlags & INT_MAX;
                        if (pBssEntry->dwFlags) {
                            if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
                                wprintf(L" - Currently connected");
                            if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE)
                                wprintf(L" - Has profile");
                        }
                        wprintf(L"\n");

                        wprintf(L"\n");
                        wifiInfos.push_back(wiFiInfo);
                    }
                }
            }
        }
    }
}
/*
int main(int argc, char* argv[])
{
    WiFiUtil* wifiUtil = new WiFiUtil();
    bool result = wifiUtil->initialize();
    if (result)
    {
        vector<WiFiInfo> wifiInfo = wifiUtil->getAvailableNetworkList("{E351C034-26EC-4596-BFCF-909172C2A32A}");
        for (vector<WiFiInfo>::iterator it = wifiInfo.begin(); it != wifiInfo.end(); ++it)
        {
            WiFiInfo wifiInfo = *it;
            cout << "SSID= " << wifiInfo.ssid << endl;
        }
        wifiUtil->connectToWiFi("SUPERONLINE_WiFi_2097");
    }
    return 0;
}*/
