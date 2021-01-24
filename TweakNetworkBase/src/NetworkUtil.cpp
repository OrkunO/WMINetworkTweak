#include "NetworkUtil.h"

NetworkUtil::NetworkUtil()
{
    pSvc = NULL;
    pLoc = NULL;
    localconn = true;
    userAcct = NULL;
}

NetworkUtil::~NetworkUtil()
{
    if (pLoc) pLoc->Release();
    if (pSvc) pSvc->Release();
    CoUninitialize();
}

bool NetworkUtil::initialize()
{
    wchar_t pszName[CREDUI_MAX_USERNAME_LENGTH + 1] = L"user";
    wchar_t pszPwd[CREDUI_MAX_PASSWORD_LENGTH + 1] = L"password";
    BSTR strNetworkResource;
    //To use a WMI remote connection set localconn to false and configure the values of the pszName, pszPwd and the name of the remote machine in strNetworkResource
    strNetworkResource = localconn ? _bstr_t(L"\\\\.\\root\\CIMV2") : _bstr_t(L"\\\\remote--machine\\root\\CIMV2");

    COAUTHIDENTITY authIdent;

    // Initialize COM. ------------------------------------------

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
        return false;                  // Program has failed.
    }

    // Set general COM security levels --------------------------
    if (localconn)
        hres = CoInitializeSecurity(
            NULL,
            -1,                          // COM authentication
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities
            NULL                         // Reserved
        );
    else
        hres = CoInitializeSecurity(
            NULL,
            -1,                          // COM authentication
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
            RPC_C_IMP_LEVEL_IDENTIFY,    // Default Impersonation
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities
            NULL                         // Reserved
        );


    if (FAILED(hres))
    {
        cout << "Failed to initialize security. Error code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
        return false;                      // Program has failed.
    }

    // Obtain the initial locator to WMI -------------------------
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object. " << "Err code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
        return false;                 // Program has failed.
    }

    // Connect to WMI through the IWbemLocator::ConnectServer method

    // Connect to the root\\CIMV2 namespace
    // and obtain pointer pSvc to make IWbemServices calls.
    if (localconn)
        hres = pLoc->ConnectServer(
            _bstr_t(strNetworkResource),      // Object path of WMI namespace
            NULL,                    // User name. NULL = current user
            NULL,                    // User password. NULL = current
            0,                       // Locale. NULL indicates current
            NULL,                    // Security flags.
            0,                       // Authority (e.g. Kerberos)
            0,                       // Context object
            &pSvc                    // pointer to IWbemServices proxy
        );
    else
        hres = pLoc->ConnectServer(
            _bstr_t(strNetworkResource),  // Object path of WMI namespace
            _bstr_t(pszName),             // User name
            _bstr_t(pszPwd),              // User password
            NULL,                // Locale
            NULL,                // Security flags
            NULL,                // Authority
            NULL,                // Context object
            &pSvc                // IWbemServices proxy
        );


    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
        return false;                // Program has failed.
    }

    cout << "Connected to root\\CIMV2 WMI namespace" << endl;

    // Set security levels on the proxy -------------------------
    if (localconn)
        hres = CoSetProxyBlanket(
            pSvc,                        // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
            NULL,                        // Server principal name
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities
        );
    else
    {
        // Create COAUTHIDENTITY that can be used for setting security on proxy
        memset(&authIdent, 0, sizeof(COAUTHIDENTITY));
        authIdent.PasswordLength = wcslen(pszPwd);
        authIdent.Password = (USHORT*)pszPwd;
        authIdent.User = (USHORT*)pszName;
        authIdent.UserLength = wcslen(pszName);
        authIdent.Domain = 0;
        authIdent.DomainLength = 0;
        authIdent.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
        userAcct = &authIdent;

        hres = CoSetProxyBlanket(
            pSvc,                           // Indicates the proxy to set
            RPC_C_AUTHN_DEFAULT,            // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_DEFAULT,            // RPC_C_AUTHZ_xxx
            COLE_DEFAULT_PRINCIPAL,         // Server principal name
            RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // RPC_C_AUTHN_LEVEL_xxx
            RPC_C_IMP_LEVEL_IMPERSONATE,    // RPC_C_IMP_LEVEL_xxx
            userAcct,                       // client identity
            EOAC_NONE                       // proxy capabilities
        );
    }

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
        return false;               // Program has failed.
    }

    return true;
}

const vector<NetworkInfo> NetworkUtil::getNetworkInfo()
{
    vector<NetworkInfo> networkInfos;
    networkInfos.clear();
    // Use the IWbemServices pointer to make requests of WMI NetworkAdapter ----
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(_bstr_t(L"WQL"), _bstr_t(L"SELECT * FROM Win32_NetworkAdapter"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

    if (FAILED(hres))
    {
        cout << "ExecQuery failed" << " Error code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
  
        NetworkInfo networkInfo;
        networkInfo.deviceId = "-1";
        networkInfo.index = -1;
        networkInfo.netConnectionID = "NoDevice";
        networkInfo.GUID = "{}";
        networkInfo.netEnabled = false;
        networkInfo.ipAddress = "0.0.0.0";
        networkInfo.subnetMask = "0.0.0.0";
        networkInfo.gateway = "0.0.0.0";
        networkInfo.dhcpEnabled = false;
        networkInfos.push_back(networkInfo);
        return networkInfos;               // Program has failed.
    }

    // Secure the enumerator proxy
    if (!localconn)
    {

        hres = CoSetProxyBlanket(
            pEnumerator,                    // Indicates the proxy to set
            RPC_C_AUTHN_DEFAULT,            // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_DEFAULT,            // RPC_C_AUTHZ_xxx
            COLE_DEFAULT_PRINCIPAL,         // Server principal name
            RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // RPC_C_AUTHN_LEVEL_xxx
            RPC_C_IMP_LEVEL_IMPERSONATE,    // RPC_C_IMP_LEVEL_xxx
            userAcct,                       // client identity
            EOAC_NONE                       // proxy capabilities
        );

        if (FAILED(hres))
        {
            cout << "Could not set proxy blanket on enumerator. Error code = 0x" << hex << hres << endl;
            cout << _com_error(hres).ErrorMessage() << endl;
            pEnumerator->Release();

            NetworkInfo networkInfo;
            networkInfo.deviceId = "-1";
            networkInfo.index = -1;
            networkInfo.netConnectionID = "NoDevice";
            networkInfo.GUID = "{}";
            networkInfo.netEnabled = false;
            networkInfo.ipAddress = "0.0.0.0";
            networkInfo.subnetMask = "0.0.0.0";
            networkInfo.gateway = "0.0.0.0";
            networkInfo.dhcpEnabled = false;
            networkInfos.push_back(networkInfo);
            return networkInfos;           // Program has failed.
        }
    }

    // Get the data from the WQL sentence
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn || FAILED(hr))
            break;

        VARIANT vtProp;
        NetworkInfo networkInfo;

        hr = pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0);// String
        if (!FAILED(hr))
        {
            if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY)) {
                wcout << "DeviceID : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                networkInfo.deviceId = "-1";
            }
            else
                if ((vtProp.vt & VT_ARRAY))
                    wcout << "DeviceID : " << "Array types not supported (yet)" << endl;
                else
                {
                    wcout << "DeviceID : " << vtProp.bstrVal << endl;
                    wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                    networkInfo.deviceId = StringHelper::ws2s(ws);
                }
  
        }
        VariantClear(&vtProp);

        hr = pclsObj->Get(L"NetConnectionID", 0, &vtProp, 0, 0);// String
        if (!FAILED(hr))
        {
            if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY)) {
                wcout << "NetConnectionID : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                networkInfo.netConnectionID = "-1";
            }
            else
                if ((vtProp.vt & VT_ARRAY))
                    wcout << "NetConnectionID : " << "Array types not supported (yet)" << endl;
                else
                {
                    wcout << "NetConnectionID : " << vtProp.bstrVal << endl;
                    wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                    networkInfo.netConnectionID = StringHelper::ws2s(ws);
                }
        }
        VariantClear(&vtProp);

        hr = pclsObj->Get(L"GUID", 0, &vtProp, 0, 0);// String
        if (!FAILED(hr))
        {
            if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                wcout << "GUID : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
            else
                if ((vtProp.vt & VT_ARRAY))
                    wcout << "GUID : " << "Array types not supported (yet)" << endl;
                else {
                    wcout << "GUID : " << vtProp.bstrVal << endl;
                    wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                    networkInfo.GUID = StringHelper::ws2s(ws);
                }
        }
        VariantClear(&vtProp);

        hr = pclsObj->Get(L"Index", 0, &vtProp, 0, 0);// Uint32
        if (!FAILED(hr))
        {
            if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY)) {
                wcout << "Index : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                networkInfo.index = -1;
            }
            else
                if ((vtProp.vt & VT_ARRAY))
                    wcout << "Index : " << "Array types not supported (yet)" << endl;
                else {
                    wcout << "Index : " << vtProp.uintVal << endl;
                    networkInfo.index = vtProp.uintVal;
                }
        }
        VariantClear(&vtProp);

        hr = pclsObj->Get(L"NetEnabled", 0, &vtProp, 0, 0);// Boolean
        if (!FAILED(hr))
        {
            if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                wcout << "NetEnabled : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
            else
                if ((vtProp.vt & VT_ARRAY))
                    wcout << "NetEnabled : " << "Array types not supported (yet)" << endl;
                else {
                    wcout << "NetEnabled : " << (vtProp.boolVal ? "True" : "False") << endl;
                    networkInfo.netEnabled = vtProp.boolVal ? true : false;
                }
        }
        VariantClear(&vtProp);

        networkInfos.push_back(networkInfo);
        pclsObj->Release();
        pclsObj = NULL;
    }

    if (networkInfos.size() > 0)
    {
        // Use the IWbemServices pointer to make requests of WMI NetworkAdapterConfiguration 
        IEnumWbemClassObject* pEnumerator = NULL;
        hres = pSvc->ExecQuery(L"WQL", L"SELECT * FROM Win32_NetworkAdapterConfiguration",
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

        if (FAILED(hres))
        {
            cout << "ExecQuery failed" << " Error code = 0x" << hex << hres << endl;
            cout << _com_error(hres).ErrorMessage() << endl;

            NetworkInfo networkInfo;
            networkInfo.deviceId = "-1";
            networkInfo.index = -1;
            networkInfo.netConnectionID = "NoDevice";
            networkInfo.GUID = "{}";
            networkInfo.netEnabled = false;
            networkInfo.ipAddress = "0.0.0.0";
            networkInfo.subnetMask = "0.0.0.0";
            networkInfo.gateway = "0.0.0.0";
            networkInfo.dhcpEnabled = false;
            networkInfos.push_back(networkInfo);
            return networkInfos;           // Program has failed.
        }

        // Secure the enumerator proxy
        if (!localconn)
        {

            hres = CoSetProxyBlanket(
                pEnumerator,                    // Indicates the proxy to set
                RPC_C_AUTHN_DEFAULT,            // RPC_C_AUTHN_xxx
                RPC_C_AUTHZ_DEFAULT,            // RPC_C_AUTHZ_xxx
                COLE_DEFAULT_PRINCIPAL,         // Server principal name
                RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // RPC_C_AUTHN_LEVEL_xxx
                RPC_C_IMP_LEVEL_IMPERSONATE,    // RPC_C_IMP_LEVEL_xxx
                userAcct,                       // client identity
                EOAC_NONE                       // proxy capabilities
            );

            if (FAILED(hres))
            {
                cout << "Could not set proxy blanket on enumerator. Error code = 0x" << hex << hres << endl;
                cout << _com_error(hres).ErrorMessage() << endl;
                pEnumerator->Release();

                NetworkInfo networkInfo;
                networkInfo.deviceId = "-1";
                networkInfo.index = -1;
                networkInfo.netConnectionID = "NoDevice";
                networkInfo.GUID = "{}";
                networkInfo.netEnabled = false;
                networkInfo.ipAddress = "0.0.0.0";
                networkInfo.subnetMask = "0.0.0.0";
                networkInfo.gateway = "0.0.0.0";
                networkInfo.dhcpEnabled = false;
                networkInfos.push_back(networkInfo);
                return networkInfos;             // Program has failed.
            }
        }

        // Get the data from the WQL sentence
        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;

        while (pEnumerator)
        {
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if (0 == uReturn || FAILED(hr))
                break;

            VARIANT vtProp;
            int deviceIndex = -1;

            hr = pclsObj->Get(L"Index", 0, &vtProp, 0, 0);// Uint32
            if (!FAILED(hr))
            {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    wcout << "Index : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                else
                    if ((vtProp.vt & VT_ARRAY))
                        wcout << "Index : " << "Array types not supported (yet)" << endl;
                    else {
                        wcout << "Index : " << vtProp.uintVal << endl;
                        deviceIndex = vtProp.uintVal;
                    }
            }
            VariantClear(&vtProp);

            NetworkInfo* networkInfo = &networkInfos[deviceIndex];

            hr = pclsObj->Get(L"DefaultIPGateway", 0, &vtProp, 0, 0);// String
            if (!FAILED(hr))
            {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    wcout << "DefaultIPGateway : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                else
                    if ((vtProp.vt & VT_ARRAY)) {
                        SAFEARRAY* psa = V_ARRAY(&vtProp);
                        BSTR* element;
                        SafeArrayAccessData(psa, (void**)&element);
                        wcout << "DefaultIPGateway First Value : " << element[0] << endl;
                        wstring ws(element[0], SysStringLen(element[0]));
                        networkInfo->gateway = StringHelper::ws2s(ws);
                        SysFreeString(element[0]);
                        element = NULL;
                        SafeArrayUnaccessData(psa);
                    }
                    else {
                        wcout << "DefaultIPGateway : " << vtProp.bstrVal << endl;

                        wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                        networkInfo->gateway = StringHelper::ws2s(ws);
                    }
            }
            VariantClear(&vtProp);

            hr = pclsObj->Get(L"DHCPEnabled", 0, &vtProp, 0, 0);// Boolean
            if (!FAILED(hr))
            {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    wcout << "DHCPEnabled : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                else
                    if ((vtProp.vt & VT_ARRAY))
                        wcout << "DHCPEnabled : " << "Array types not supported (yet)" << endl;
                    else {
                        wcout << "DHCPEnabled : " << (vtProp.boolVal ? "True" : "False") << endl;
                        networkInfo->dhcpEnabled = vtProp.boolVal ? true : false;
                    }
            }
            VariantClear(&vtProp);

            hr = pclsObj->Get(L"DHCPServer", 0, &vtProp, 0, 0);// String
            if (!FAILED(hr))
            {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    wcout << "DHCPServer : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                else
                    if ((vtProp.vt & VT_ARRAY)) {
                        SAFEARRAY* psa = V_ARRAY(&vtProp);
                        BSTR* element;
                        SafeArrayAccessData(psa, (void**)&element);
                        wcout << "DHCPServer First Value : " << element[0] << endl;
                        wstring ws(element[0], SysStringLen(element[0]));
                        networkInfo->dhcpServer = StringHelper::ws2s(ws);
                        SysFreeString(element[0]);
                        element = NULL;
                        SafeArrayUnaccessData(psa);
                    }
                    else {
                        wcout << "DHCPServer : " << vtProp.bstrVal << endl;
                        wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                        networkInfo->dhcpServer = StringHelper::ws2s(ws);
                    }
            }
            VariantClear(&vtProp);

            hr = pclsObj->Get(L"IPAddress", 0, &vtProp, 0, 0);// String
            if (!FAILED(hr))
            {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    wcout << "IPAddress : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                else
                    if ((vtProp.vt & VT_ARRAY)) {
                        SAFEARRAY* psa = V_ARRAY(&vtProp);
                        BSTR* element;
                        SafeArrayAccessData(psa, (void**)&element);
                        wcout << "IPAddress First Value : " << element[0] << endl;
                        wstring ws(element[0], SysStringLen(element[0]));
                        networkInfo->ipAddress = StringHelper::ws2s(ws);
                        SysFreeString(element[0]);
                        element = NULL;
                        SafeArrayUnaccessData(psa);
                    }
                    else {
                        wcout << "IPAddress : " << vtProp.bstrVal << endl;
                        wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                        networkInfo->ipAddress = StringHelper::ws2s(ws);
                    }
            }
            VariantClear(&vtProp);

            hr = pclsObj->Get(L"IPSubnet", 0, &vtProp, 0, 0);// String
            if (!FAILED(hr))
            {
                if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    wcout << "IPSubnet : " << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                else
                    if ((vtProp.vt & VT_ARRAY)) {
                        SAFEARRAY* psa = V_ARRAY(&vtProp);
                        BSTR* element;
                        SafeArrayAccessData(psa, (void**)&element);
                        wcout << "IPSubnet First Value : " << element[0] << endl;
                        wstring ws(element[0], SysStringLen(element[0]));
                        networkInfo->subnetMask = StringHelper::ws2s(ws);
                        SysFreeString(element[0]);
                        element = NULL;
                        SafeArrayUnaccessData(psa);
                    }
                    else {
                        wcout << "IPSubnet : " << vtProp.bstrVal << endl;
                        wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
                        networkInfo->ipAddress = StringHelper::ws2s(ws);
                    }
            }
            VariantClear(&vtProp);

            pclsObj->Release();
            pclsObj = NULL;
        }

        // Cleanup
        pEnumerator->Release();
        if (pclsObj != NULL)
            pclsObj->Release();
    }

    return networkInfos;   // Program successfully completed.
}

bool NetworkUtil::tweakNetwork(bool enable, string deviceId)
{
    // Use the IWbemServices pointer to make requests of WMI 
    BSTR MethodName = SysAllocString(enable ? L"Enable" : L"Disable");
    char classNameString[100];
    wchar_t w_classNameStringString[100];
    strcpy_s(classNameString, "Win32_NetworkAdapter.DeviceID=\"");
    strcat_s(classNameString, deviceId.c_str());
    strcat_s(classNameString, "\"");
    mbstowcs(w_classNameStringString, classNameString, 100);
    BSTR ClassName = SysAllocString(w_classNameStringString);
    wcout << "Win DeviceId " << ClassName << endl;

    // Execute Method
    IWbemClassObject* pOutParams = NULL;
    hres = pSvc->ExecMethod(ClassName, MethodName, 0,
        NULL, NULL, &pOutParams, NULL);

    if (FAILED(hres))
    {
        cout << "Could not execute method. Error code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
        SysFreeString(MethodName);

        if (pOutParams) pOutParams->Release();
        return false;               // Program has failed.
    }

    VARIANT varReturnValue;
    hres = pOutParams->Get(L"ReturnValue", 0, &varReturnValue, NULL, 0);
    if (!FAILED(hres))
        wcout << "ReturnValue " << varReturnValue.intVal << endl;
    VariantClear(&varReturnValue);

    // Clean up    
    SysFreeString(MethodName);
    SysFreeString(ClassName);
    if (pOutParams) pOutParams->Release();
   
    return true;
}

bool NetworkUtil::enableStaticWithGateway(UINT deviceIndex, string ipAddress, string subnetMask, string gateway)
{
    // Use the IWbemServices pointer to make requests of WMI ----
    HRESULT hr;
    // Grab class required to work on Win32_NetworkAdapterConfiguration
    IWbemClassObject* pClass = NULL;
    BSTR ClassPath = SysAllocString(L"Win32_NetworkAdapterConfiguration");
    hr = pSvc->GetObject(ClassPath, 0, NULL, &pClass, NULL);
    SysFreeString(ClassPath);
    if (WBEM_S_NO_ERROR == hr)
    {
        // Grab pointers to the input parameter class of the two methods we are going to call
        BSTR MethodName_ES = SysAllocString(L"EnableStatic");
        BSTR MethodName_SG = SysAllocString(L"SetGateways");
        IWbemClassObject* pInClass_ES = NULL;
        IWbemClassObject* pInClass_SG = NULL;
        if (WBEM_S_NO_ERROR == pClass->GetMethod(MethodName_ES, 0, &pInClass_ES, NULL) &&
            WBEM_S_NO_ERROR == pClass->GetMethod(MethodName_SG, 0, &pInClass_SG, NULL))
        {
            // Spawn instances of the input parameter classes, so that we can stuff our parameters in
            IWbemClassObject* pInInst_ES = NULL;
            IWbemClassObject* pInInst_SG = NULL;

            if (WBEM_S_NO_ERROR == pInClass_ES->SpawnInstance(0, &pInInst_ES) &&
                WBEM_S_NO_ERROR == pInClass_SG->SpawnInstance(0, &pInInst_SG))
            {
                // Convert from multibyte strings to wide character arrays
                wchar_t tmp_ip[40];
                wchar_t tmp_netmask[40];
                wchar_t tmp_gateway[40];
                unsigned short metric = 5;

                mbstowcs(tmp_ip, ipAddress.c_str(), 40);
                mbstowcs(tmp_netmask, subnetMask.c_str(), 40);
                mbstowcs(tmp_gateway, gateway.c_str(), 40);

                // Insert into safe arrays, allocating memory as we do so (destroying the safe array will destroy the allocated memory)
                long index[] = { 0 };

                BSTR ip = SysAllocString(tmp_ip);
                BSTR netmask = SysAllocString(tmp_netmask);
                BSTR gateway = SysAllocString(tmp_gateway);

                wcout << "Enable Static IP Address= " << ip << endl;
                wcout << "Enable Static Subnet Mask= " << netmask << endl;
                wcout << "Enable Static Gateway= " << gateway << endl;

                SAFEARRAY* ip_list = SafeArrayCreateVector(VT_BSTR, 0, 1);
                SafeArrayPutElement(ip_list, index, ip);

                SAFEARRAY* mask_list = SafeArrayCreateVector(VT_BSTR, 0, 1);
                SafeArrayPutElement(mask_list, index, netmask);

                SAFEARRAY* gateway_list = SafeArrayCreateVector(VT_BSTR, 0, 1);
                SafeArrayPutElement(gateway_list, index, gateway);

                SAFEARRAY* metric_list = SafeArrayCreateVector(VT_UI1, 0, 1);
                SafeArrayPutElement(metric_list, index, &metric);

                // Now wrap each safe array in a VARIANT so that it can be passed to COM function
                VARIANT arg1_ES;
                arg1_ES.vt = VT_ARRAY | VT_BSTR;
                arg1_ES.parray = ip_list;

                VARIANT arg2_ES;
                arg2_ES.vt = VT_ARRAY | VT_BSTR;
                arg2_ES.parray = mask_list;

                VARIANT arg1_SG;
                arg1_SG.vt = VT_ARRAY | VT_BSTR;
                arg1_SG.parray = gateway_list;

                VARIANT arg2_SG;
                arg2_SG.vt = VT_ARRAY | VT_UI1;
                arg2_SG.parray = metric_list;

                if (WBEM_S_NO_ERROR == pInInst_ES->Put(L"IPAddress", 0, &arg1_ES, 0) &&
                    WBEM_S_NO_ERROR == pInInst_ES->Put(L"SubNetMask", 0, &arg2_ES, 0) &&
                    WBEM_S_NO_ERROR == pInInst_SG->Put(L"DefaultIPGateway", 0, &arg1_SG, 0) &&
                    WBEM_S_NO_ERROR == pInInst_SG->Put(L"GatewayCostMetric", 0, &arg2_SG, 0))
                {
                    // First build the object path that specifies which network adapter we are executing a method on
                    char indexString[10];
                    _itoa_s(deviceIndex, indexString, 10);

                    char instanceString[100];
                    wchar_t w_instanceString[100];
                    strcpy_s(instanceString, "Win32_NetworkAdapterConfiguration.Index='");
                    strcat_s(instanceString, indexString);
                    strcat_s(instanceString, "'");
                    mbstowcs(w_instanceString, instanceString, 100);
                    BSTR InstancePath = SysAllocString(w_instanceString);
                    wcout << "InstancePath: " << InstancePath << endl;
                    /*
                    _variant_t cn;

                    hr = pInInst_ES->Get(L"IPAddress", 0, &cn, NULL, NULL);
                    if (SUCCEEDED(hr))
                    {
                        if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY))
                            wcout << "IPAddress : " << ((cn.vt == VT_NULL) ? "NULL" : "EMPTY") << endl;
                        else {
                            SAFEARRAY* psa = V_ARRAY(&cn);
                            BSTR* element;
                            SafeArrayAccessData(psa, (void**)&element);
                            wcout << "IPAddress First Value : " << element[0] << endl;
                            SysFreeString(element[0]);
                            element = NULL;
                            SafeArrayUnaccessData(psa);
                            //wcout << "IPAddress : " << cn.bstrVal << endl;
                        }
                    }
                    VariantClear(&cn);*/

                    // Now call the methods
                    IWbemClassObject* pOutInst = NULL;
                    hr = pSvc->ExecMethod(InstancePath, MethodName_ES, 0, NULL, pInInst_ES, &pOutInst, NULL);
                    if (WBEM_S_NO_ERROR == hr)
                        wcout << "SetAdapterConfig - Enabled static IP successfully." << endl;
                    else
                        wcout << "SetAdapterConfig - Could not enable static IP - 0x%0x" << hr << endl;

                    hr = pSvc->ExecMethod(InstancePath, MethodName_SG, 0, NULL, pInInst_SG, NULL, NULL);
                    if (WBEM_S_NO_ERROR == hr)
                        wcout << "SetAdapterConfig - Set gateways successfully." << endl;
                    else
                        wcout << "SetAdapterConfig - Could not set gateways - 0x%0x" << hr << endl;

                    SysFreeString(InstancePath);
                }

                // Clear the variants - does this actually get ride of safearrays?
                VariantClear(&arg1_ES);
                VariantClear(&arg2_ES);
                VariantClear(&arg1_SG);
                VariantClear(&arg2_SG);

                // Destroy safe arrays, which destroys the objects stored inside them
                //SafeArrayDestroy(ip_list); ip_list = NULL;
                //SafeArrayDestroy(mask_list); mask_list = NULL;
                //SafeArrayDestroy(gateway_list); gateway_list = NULL;

                // Destroy the BSTR pointers
                SysFreeString(ip);
                SysFreeString(netmask);
                SysFreeString(gateway);
            }

            // Free up the instances that we spawned
            if (pInInst_ES)
            {
                pInInst_ES->Release();
                pInInst_ES = NULL;
            }
            if (pInInst_SG)
            {
                pInInst_SG->Release();
                pInInst_SG = NULL;
            }
        }

        // Free up methods input parameters class pointers
        if (pInClass_ES)
        {
            pInClass_ES->Release();
            pInClass_ES = NULL;
        }
        if (pInClass_SG)
        {
            pInClass_SG->Release();
            pInClass_SG = NULL;
        }

        SysFreeString(MethodName_ES);
        SysFreeString(MethodName_SG);
    }

    // Variable cleanup
    if (pClass)
    {
        pClass->Release();
        pClass = NULL;
    }
    return true;
}

bool NetworkUtil::enableDHCP(UINT deviceIndex)
{
    // Use the IWbemServices pointer to make requests of WMI ----

    BSTR MethodName = SysAllocString(L"EnableDHCP");
    BSTR ClassName = SysAllocString(L"Win32_NetworkAdapterConfiguration");

    // First build the object path that specifies which network adapter we are executing a method on
    char indexString[10];
    _itoa_s(deviceIndex, indexString, 10);

    char instanceString[100];
    wchar_t w_instanceString[100];
    strcpy_s(instanceString, "Win32_NetworkAdapterConfiguration.Index='");
    strcat_s(instanceString, indexString);
    strcat_s(instanceString, "'");
    mbstowcs(w_instanceString, instanceString, 100);
    BSTR InstancePath = SysAllocString(w_instanceString);

    // Execute Method
    IWbemClassObject* pOutParams = NULL;
    hres = pSvc->ExecMethod(InstancePath, MethodName, 0,
        NULL, NULL, &pOutParams, NULL);

    if (FAILED(hres))
    {
        cout << "Could not execute method. Error code = 0x" << hex << hres << endl;
        cout << _com_error(hres).ErrorMessage() << endl;
        SysFreeString(ClassName);
        SysFreeString(MethodName);
        if (pOutParams) pOutParams->Release();
        return false;               // Program has failed.
    }


    VARIANT varReturnValue;
    hres = pOutParams->Get(L"ReturnValue", 0, &varReturnValue, NULL, 0);
    if (!FAILED(hres))
        wcout << "ReturnValue " << varReturnValue.intVal << endl;
    VariantClear(&varReturnValue);


    // Clean up    
    SysFreeString(ClassName);
    SysFreeString(MethodName);
    if (pOutParams)	pOutParams->Release();

    return true;
}
/*
int main(int argc, char* argv[])
{
    NetworkUtil* networkUtil = new NetworkUtil();
    cout << "initialize 2..." << endl;
    if (networkUtil->initialize())
    {
        cout << "initialize 2..." << endl;
        vector<NetworkInfo> networkInfos = networkUtil->getNetworkInfo();

        //for (vector<NetworkInfo>::iterator it = networkInfos.begin(); it != networkInfos.end(); ++it)
        //{
        //    NetworkInfo networkInfo = *it;
        //    cout << "Device Id: " << networkInfo.deviceId << " - Net Connection ID: " << networkInfo.netConnectionID << endl;
        //    cout << "IP Address: " << networkInfo.ipAddress << " - Subnet Mask: " << networkInfo.subnetMask << endl;
        //    if (networkInfo.deviceId == "4")
        //    {
        //        //bool result = networkUtil->tweakNetwork(true, networkInfo.deviceId);
        //        //cout << "Disable Result: " << result << endl;
        //        bool result = networkUtil->enableStaticWithGateway(networkInfo.index, "192.168.1.103", "255.255.255.0", "192.168.1.1");
        //        cout << "Enable Static with Gateway Result: " << result << endl;
        //        //bool result = networkUtil->enableDHCP(networkInfo.index);
        //        //cout << "Enable DHCP Result: " << result << endl;
        //        break;
        //    }
        //}
        cout << "press enter to exit" << endl;
        cin.get();
    }
    return 0;
}
*/

