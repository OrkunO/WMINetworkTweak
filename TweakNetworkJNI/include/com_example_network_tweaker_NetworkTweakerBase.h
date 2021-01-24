/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include "NetworkUtil.h"
#include "WiFiUtil.h"
/* Header for class com_example_network_tweaker_NetworkTweakerBase */

#ifndef _Included_com_example_network_tweaker_NetworkTweakerBase
#define _Included_com_example_network_tweaker_NetworkTweakerBase
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    getNetworkInfo
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_getNetworkInfo
  (JNIEnv *, jobject);

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    initialize
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_initialize
  (JNIEnv *, jobject);

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    tweakNetwork
 * Signature: (ZLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_tweakNetwork
  (JNIEnv *, jobject, jboolean, jstring);

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    enableStaticWithGateway
 * Signature: (ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_enableStaticWithGateway
  (JNIEnv *, jobject, jint, jstring, jstring, jstring);

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    enableDHCP
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_enableDHCP
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    wifiInitialize
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_wifiInitialize
(JNIEnv*, jobject);

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    wifiGetAvailableNetworkList
 * Signature: (Ljava/lang/String;)Ljava/util/ArrayList;
 */
JNIEXPORT jobject JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_wifiGetAvailableNetworkList
(JNIEnv*, jobject, jstring);

/*
 * Class:     com_example_network_tweaker_NetworkTweakerBase
 * Method:    wifiConnect
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_network_tweaker_NetworkTweakerBase_wifiConnect
(JNIEnv*, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
