#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <iostream>
using namespace std;
class NetworkInfo {

public:
    string deviceId;
    int index;
    string netConnectionID;
    string GUID;
    bool netEnabled;
    string ipAddress;
    string subnetMask;
    string gateway;
    string dhcpServer;
    bool dhcpEnabled;
};
#endif
