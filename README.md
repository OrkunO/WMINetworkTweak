# WMI Network Tweak with JNI

##### Table of Contents  
[Introduction](#introduction)\
[Requirements](#requirements)\
[Build](#build)\
[References](#references)\
<a name="introduction"/>
## Introduction
Windows Management Instrumentation (WMI) is the Microsoft implementation of Web-Based Enterprise Management (WBEM), which is an industry initiative to develop a standard technology for accessing management information in an enterprise environment. WMI uses the Common Information Model (CIM) industry standard to represent systems, applications, networks, devices, and other managed components.

Tweak Network is a JavaFX application which uses JNI(Java Native Interface) to access the WMI. WMI **"Win32_NetworkAdapter"** and **"Win32_NetworkAdapterConfiguration"** interfaces are used for changing network adapter settings. In addition, available Wi-Fi networks for Wi-Fi adapters (if available) can be viewed and connected to any desired Wi-Fi network. **(wlanapi)** 

Wireless network adapter cannot be determined due to a bug from "Win32_NetworkAdapter_AdapterType" and "Win32_NetworkAdapter_AdapterTypeId". "NetConnectionID" parameter is used for getting the Wi-Fi adapter.


**Project Structure**
* **NetworkTweakerApp:** Maven-JavaFX project for the Tweak Network UI. **"launch4j-maven-plugin**" is used because of the administrative rights of the WMI Network operations. Uses Tweak Network JNI library for the following operations:
  * Get Network Adapters
  * Enable/Disable Network Adapter
  * Enable Static/Gateway
  * Enable DHCP
  * Show Available Wi-Fi Networks
  * Connect to Wi-Fi Network
* **TweakNetworkBase:** C++ implementations of the "Win32_NetworkAdapter", "Win32_NetworkAdapterConfiguration" properties and functions, including Wi-Fi operations with using "wlanapi".
* **TweakNetworkJNI:** JNI bridge for accessing C++ WMI interface through Java.
## Requirements
* Java 8 + (Tested with 1.8.0_112)
* CMake - TweakNetworkBase and TweakNetworkJNI projects use CMake for creating JNI library. (Tested with 3.19.0)
* Visual Studio 16 2019
- - - -
### Build
Execute the following commands from the root of the project to build the tweak network JNI dll.
```bash
cmake -G "Visual Studio 16 2019" -A x64 -S . -B "build64"
cmake --build build64 --config Release
```
JNI library can be seen in the "NetworkTweakerApp" build directory.
```bash
cd .\NetworkTweakerApp\
mvn install
```
"launch4j-maven-plugin" will genereate "tweaker.exe" in the "NetworkTweakerApp" build directory.

## References
[https://docs.microsoft.com/en-us/windows/win32/wmisdk/about-wmi](https://docs.microsoft.com/en-us/windows/win32/wmisdk/about-wmi)
