#include "com_example_network_tweaker_NetworkTweakerBase.h"
#include <string>

NetworkUtil* networkUtil;
WiFiUtil* wifiUtil;
/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    getNetworkInfo
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_getNetworkInfo
(JNIEnv* env, jobject cls)
{
    printf("Get Network Info Begin... \n");
    jclass java_util_ArrayList = static_cast<jclass>(env->FindClass("java/util/ArrayList"));

    jmethodID java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList, "<init>", "(I)V");
    jmethodID java_util_ArrayList_add = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
    //jmethodID java_util_ArrayList_get = env->GetMethodID(java_util_ArrayList, "get", "(I)Ljava/lang/Object;");
    
    jclass networkTweakerCls = static_cast<jclass>(env->FindClass("com/example/network/tweaker/NetworkTweaker"));
    if (networkTweakerCls == NULL) {
        printf("cannot find networkTweaker class... \n");
    }
    jclass networkInfoCls = static_cast<jclass>(env->FindClass("com/example/network/tweaker/NetworkTweaker$NetworkInfo"));
    if (networkInfoCls == NULL) {
        printf("cannot find networkinfo class... \n");
    } 

    //jmethodID method = env->GetMethodID(networkInfoCls, "<init>", "(Lcom/example/network/tweaker/NetworkTweaker;Ljava/lang/String;Ljava/lang/Integer;Ljava/lang/String;)V");
    //if (method == NULL) {
    //    printf("cannot find constructor of the class... \n");
    //}

    jmethodID setDeviceIdMethod = env->GetMethodID(networkInfoCls, "setDeviceId", "(Ljava/lang/String;)V");
    jmethodID setDeviceIndexMethod = env->GetMethodID(networkInfoCls, "setDeviceIndex", "(I)V");
    jmethodID setNetConnectionIdMethod = env->GetMethodID(networkInfoCls, "setNetConnectionId", "(Ljava/lang/String;)V");
    jmethodID setGuidMethod = env->GetMethodID(networkInfoCls, "setGuid", "(Ljava/lang/String;)V");
    jmethodID setNetEnabledMethod = env->GetMethodID(networkInfoCls, "setNetEnabled", "(Z)V");
    jmethodID setDhcpEnabledMethod = env->GetMethodID(networkInfoCls, "setDhcpEnabled", "(Z)V");
    jmethodID setIpAddressMethod = env->GetMethodID(networkInfoCls, "setIpAddress", "(Ljava/lang/String;)V");
    jmethodID setSubnetMaskMethod = env->GetMethodID(networkInfoCls, "setSubnetMask", "(Ljava/lang/String;)V");
    jmethodID setGatewayMethod = env->GetMethodID(networkInfoCls, "setGateway", "(Ljava/lang/String;)V");
    jmethodID setDhcpServerMethod = env->GetMethodID(networkInfoCls, "setDhcpServer", "(Ljava/lang/String;)V");

    printf("Get Network Info - JNetworkInfo Constructor... \n");

    vector<NetworkInfo> networkInfos =  networkUtil->getNetworkInfo();
    jobject objArray = static_cast<jobject>(env->NewObject(java_util_ArrayList, java_util_ArrayList_, networkInfos.size()));
    printf("Get Network Info - ArrayList New... \n");
    for (vector<NetworkInfo>::iterator it = networkInfos.begin(); it != networkInfos.end(); ++it)
    {
        NetworkInfo networkInfo = *it;
        
        printf("Incoming Network Info - Device ID: %s\n", networkInfo.deviceId.c_str());
        printf("Incoming Network Info - Index: %d\n", networkInfo.index);
        printf("Incoming Network Info - Net Connection ID: %s\n", networkInfo.netConnectionID.c_str());
        printf("Incoming Network Info - GUID: %s\n", networkInfo.GUID.c_str());
        printf("Incoming Network Info - Net Enabled: %d\n", networkInfo.netEnabled);
        printf("Incoming Network Info - IP Address: %s\n", networkInfo.ipAddress.c_str());
        printf("Incoming Network Info - Subnet Mask: %s\n", networkInfo.subnetMask.c_str());
        printf("Incoming Network Info - Gateway: %s\n", networkInfo.gateway.c_str());
        printf("Incoming Network Info - DHCP Server: %s\n", networkInfo.dhcpServer.c_str());
        printf("Incoming Network Info - DHCP Enabled: %d\n", networkInfo.dhcpEnabled);

        jstring strDeviceId = env->NewStringUTF(networkInfo.deviceId.c_str());
        jstring strNetConnectionId = env->NewStringUTF(networkInfo.netConnectionID.c_str());
        jstring strGUID = env->NewStringUTF(networkInfo.GUID.c_str());
        jboolean bNetEnabled = networkInfo.netEnabled ? JNI_TRUE : JNI_FALSE;
        jstring strIpAddress = env->NewStringUTF(networkInfo.ipAddress.c_str());
        jstring strSubnetMask = env->NewStringUTF(networkInfo.subnetMask.c_str());
        jstring strGateway = env->NewStringUTF(networkInfo.gateway.c_str());
        jstring strDhcpServer = env->NewStringUTF(networkInfo.dhcpServer.c_str());
        jboolean bDhcpEnabled = networkInfo.dhcpEnabled ? JNI_TRUE : JNI_FALSE;
        printf("Device ID - Net Connection ID - Index Conversion Completed...\n");
        //jobject jNetworkInfo = env->NewObject(networkInfoCls, method, strDeviceId, networkInfo.index, strNetConnectionId);
        jobject jNetworkInfo = env->AllocObject(networkInfoCls);
        printf("Alloc New Object \n");
        env->CallObjectMethod(jNetworkInfo, setDeviceIdMethod, strDeviceId);
        printf("New Object Set Device Id Method \n");
        env->CallObjectMethod(jNetworkInfo, setDeviceIndexMethod, networkInfo.index);
        printf("New Object Set Device Index Method \n");
        env->CallObjectMethod(jNetworkInfo, setNetConnectionIdMethod, strNetConnectionId);
        printf("New Object Set Net Connection Id Method \n");
        env->CallObjectMethod(jNetworkInfo, setGuidMethod, strGUID);
        printf("New Object Set GUID Method \n");
        env->CallObjectMethod(jNetworkInfo, setNetEnabledMethod, bNetEnabled);
        printf("New Object Net Enabled Method \n");
        env->CallObjectMethod(jNetworkInfo, setDhcpEnabledMethod, bDhcpEnabled);
        printf("New Object DHCP Enabled Method \n");
        env->CallObjectMethod(jNetworkInfo, setIpAddressMethod, strIpAddress);
        printf("New Object Set IP Address Method \n");
        env->CallObjectMethod(jNetworkInfo, setSubnetMaskMethod, strSubnetMask);
        printf("New Object Set Subnet Mask Method \n");
        env->CallObjectMethod(jNetworkInfo, setGatewayMethod, strGateway);
        printf("New Object Set Gateway Method \n");
        env->CallObjectMethod(jNetworkInfo, setDhcpServerMethod, strDhcpServer);
        printf("New Object Set DHCP Server Method \n");
        printf("New JNetworkInfo Object...\n");
        jboolean result = env->CallBooleanMethod(objArray, java_util_ArrayList_add, jNetworkInfo);
        printf("Added Network Info to ArrayList \n");
        //jobject element = static_cast<jobject>(env->CallObjectMethod(objArray, java_util_ArrayList_get, 0));
        //printf("Get Network Info from ArrayList \n");
        //jclass clss = env->GetObjectClass(element);
        //// First get the class object
        //printf("Get Network Info Object Class \n");
        //jmethodID mid = env->GetMethodID(clss, "getClass", "()Ljava/lang/Class;");
        //printf("Get Network Info Class Method ID \n");
        //jobject clsObj = env->CallObjectMethod(element, mid);
        //printf("Get Network Info Class Call Object Method \n");
        //
        //// Now get the class object's class descriptor
        //jclass clsss = env->GetObjectClass(clsObj);
        //printf("Get Network Info Class Get Object Class \n");
        //
        //// Find the getName() method on the class object
        //mid = env->GetMethodID(clsss, "getName", "()Ljava/lang/String;");
        //printf("Get Network Info Class Find Class Name \n");
        //
        //// Call the getName() to get a jstring object back
        //jstring strObj = (jstring)env->CallObjectMethod(clsObj, mid);
        //printf("Get Network Info Class Call Get Name Object Method\n");
        //
        //// Now get the c string from the java jstring object
        //const char* str = env->GetStringUTFChars(strObj, NULL);
        //
        //// Print the class name
        //printf("\nCalling class is: %s\n", str);
        //
        //// Release the memory pinned char array
        //env->ReleaseStringUTFChars(strObj, str);
        //
        //jmethodID a = env->GetMethodID(clss, "getDeviceIndex", "()I");
        //jint iVal = (jint)env->CallObjectMethod(element, a);
        //
        //
        //
        //printf("index: %d\n", iVal);
        env->DeleteLocalRef(jNetworkInfo);
        env->DeleteLocalRef(strNetConnectionId);
        env->DeleteLocalRef(strGUID);
        env->DeleteLocalRef(strIpAddress);
        env->DeleteLocalRef(strSubnetMask);
        env->DeleteLocalRef(strGateway);
        env->DeleteLocalRef(strDhcpServer);
    }

    return objArray;
}

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    initialize
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_initialize
(JNIEnv* env, jobject cls)
{
    networkUtil = new NetworkUtil();
    return networkUtil->initialize();
}

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    tweakNetwork
 * Signature: (ZLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_tweakNetwork
(JNIEnv* env, jobject cls, jboolean enable, jstring deviceId)
{
    const char* strDeviceId;
    strDeviceId = env->GetStringUTFChars(deviceId, NULL);
    if (strDeviceId == NULL) {
        printf("One or more Passed Parameters Empty...\n");
        return false;
    }

    printf("Passed DeviceId: %s\n", strDeviceId);
    printf("Passed Enable: %d\n", enable == JNI_TRUE ? 1 : 0);

    bool result = networkUtil->tweakNetwork(enable, strDeviceId);
    // after using it, remember to release the memory
    env->ReleaseStringUTFChars(deviceId, strDeviceId);
    return result;
}

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    enableStaticWithGateway
 * Signature: (ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_enableStaticWithGateway
(JNIEnv* env, jobject cls, jint deviceIndex, jstring ipAddress, jstring subnetMask, jstring gateway) {
    const char* strIpAddress;
    const char* strSubnetMask;
    const char* strGateway;
    strIpAddress = env->GetStringUTFChars(ipAddress, NULL);
    strSubnetMask = env->GetStringUTFChars(subnetMask, NULL);
    strGateway = env->GetStringUTFChars(gateway, NULL);
    if (strIpAddress == NULL || strSubnetMask == NULL || strGateway == NULL) {
        printf("One or more Passed Parameters Empty...\n");
        return false;
    }

    printf("Passed Device Index: %d\n", deviceIndex);
    printf("Passed IP Address: %s\n", strIpAddress);
    printf("Passed Subnet Mask: %s\n", strSubnetMask);
    printf("Passed Gateway: %s\n", strGateway);

    bool result = networkUtil->enableStaticWithGateway(deviceIndex, strIpAddress, strSubnetMask, strGateway);
    env->ReleaseStringUTFChars(ipAddress, strIpAddress);
    env->ReleaseStringUTFChars(subnetMask, strSubnetMask);
    env->ReleaseStringUTFChars(gateway, strGateway);
    return result;
}

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    enableDHCP
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_enableDHCP
(JNIEnv* env, jobject cls, jint deviceIndex) {
    printf("Passed Device Index: %d\n", deviceIndex);
    return networkUtil->enableDHCP(deviceIndex);
}

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    wifiInitialize
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_wifiInitialize
(JNIEnv* env, jobject cls) {
    wifiUtil = new WiFiUtil();
    return wifiUtil->initialize();
}

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    wifiGetAvailableNetworkList
 * Signature: (Ljava/lang/String;)Ljava/util/ArrayList;
 */
JNIEXPORT jobject JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_wifiGetAvailableNetworkList
(JNIEnv* env, jobject cls, jstring interfaceGUID) {
    const char* strInterfaceGUID;
    strInterfaceGUID = env->GetStringUTFChars(interfaceGUID, NULL);

    jclass java_util_ArrayList = static_cast<jclass>(env->FindClass("java/util/ArrayList"));

    jmethodID java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList, "<init>", "(I)V");
    jmethodID java_util_ArrayList_add = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");

    jclass networkTweakerCls = static_cast<jclass>(env->FindClass("com/example/network/tweaker/NetworkTweaker"));
    if (networkTweakerCls == NULL) {
        printf("cannot find networkTweaker class... \n");
    }
    jclass wiFiInfoCls = static_cast<jclass>(env->FindClass("com/example/network/tweaker/NetworkTweaker$WiFiInfo"));
    if (wiFiInfoCls == NULL) {
        printf("cannot find wiFiInfoCls class... \n");
    }

    jmethodID setSsidMethod = env->GetMethodID(wiFiInfoCls, "setSsid", "(Ljava/lang/String;)V");
    jmethodID setRssiMethod = env->GetMethodID(wiFiInfoCls, "setRssi", "(I)V");
    jmethodID setFlagsMethod = env->GetMethodID(wiFiInfoCls, "setFlags", "(J)V");
   
    vector<WiFiInfo> wiFiInfos = wifiUtil->getAvailableNetworkList(strInterfaceGUID);
    jobject objArray = static_cast<jobject>(env->NewObject(java_util_ArrayList, java_util_ArrayList_, wiFiInfos.size()));
    for (vector<WiFiInfo>::iterator it = wiFiInfos.begin(); it != wiFiInfos.end(); ++it)
    {
        WiFiInfo wiFiInfo = *it;
        jstring strSsid = env->NewStringUTF(wiFiInfo.ssid.c_str());
        jint rssi = wiFiInfo.iRRSI;
        jlong flags = wiFiInfo.flags;

        jobject jWiFiInfo = env->AllocObject(wiFiInfoCls);

        env->CallObjectMethod(jWiFiInfo, setSsidMethod, strSsid);
        env->CallObjectMethod(jWiFiInfo, setRssiMethod, rssi);
        env->CallObjectMethod(jWiFiInfo, setFlagsMethod, flags);

        jboolean result = env->CallBooleanMethod(objArray, java_util_ArrayList_add, jWiFiInfo);

        env->DeleteLocalRef(jWiFiInfo);
        env->DeleteLocalRef(strSsid);
    }

    env->ReleaseStringUTFChars(interfaceGUID, strInterfaceGUID);
    return objArray;
}

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    wifiConnect
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_wifiConnect
(JNIEnv* env, jobject cls, jstring ssid) {
    const char* strSSID;
    strSSID = env->GetStringUTFChars(ssid, NULL);

    bool result = wifiUtil->connectToWiFi(strSSID);

    env->ReleaseStringUTFChars(ssid, strSSID);
    return result;
}

