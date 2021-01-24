#ifndef WIFIUTIL_H
#define WIFIUTIL_H

#ifndef UNICODE
#define UNICODE
#endif

#include "StringHelper.h"
#include "WiFiInfo.h"
#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <string>
#include <vector> 
#include <iostream>
#include <climits>

#include <stdio.h>
#include <stdlib.h>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

using namespace std;
class WiFiUtil
{
public:
    HANDLE hClient;
    PWLAN_INTERFACE_INFO pIfInfo;
    PWLAN_INTERFACE_INFO_LIST pIfList;
    PWLAN_AVAILABLE_NETWORK_LIST pBssList;
    PWLAN_AVAILABLE_NETWORK pBssEntry;

    DWORD dwRetVal;
    DWORD dwResult;
    DWORD dwMaxClient;
    DWORD dwCurVersion;
   
    WiFiUtil();
    ~WiFiUtil();

    bool initialize();
    const vector<WiFiInfo> getAvailableNetworkList(string interfaceGUID);
    bool connectToWiFi(string wifiSSID);
    
};

#endif
