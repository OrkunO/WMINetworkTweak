#ifndef NETWORKUTIL_H
#define NETWORKUTIL_H

#include "NetworkInfo.h"
#include "StringHelper.h"
#include <vector>
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
# pragma comment(lib, "wbemuuid.lib")

//CREDENTIAL structure
//http://msdn.microsoft.com/en-us/library/windows/desktop/aa374788%28v=vs.85%29.aspx
#define CRED_MAX_USERNAME_LENGTH            513
#define CRED_MAX_CREDENTIAL_BLOB_SIZE       512
#define CREDUI_MAX_USERNAME_LENGTH CRED_MAX_USERNAME_LENGTH
#define CREDUI_MAX_PASSWORD_LENGTH (CRED_MAX_CREDENTIAL_BLOB_SIZE / 2)

using namespace std;
class NetworkUtil
{
public:
    HRESULT hres;
    IWbemServices* pSvc;
    IWbemLocator* pLoc;
    bool localconn;
    COAUTHIDENTITY* userAcct;

    NetworkUtil(); 
    ~NetworkUtil();

    const vector<NetworkInfo> getNetworkInfo();
    bool initialize();
    bool tweakNetwork(bool enable, string deviceId);
    bool enableStaticWithGateway(UINT deviceIndex, string ipAddress, string subnetMask, string gateway);
    bool enableDHCP(UINT deviceIndex);
};

#endif
