package com.example.network.tweaker.util;

public enum NetworkFlagsEnum {

    WLAN_AVAILABLE_NETWORK(0),
    WLAN_AVAILABLE_NETWORK_CONNECTED(1),  // This network is currently connected
    WLAN_AVAILABLE_NETWORK_HAS_PROFILE(2), // There is a profile for this network
    WLAN_AVAILABLE_NETWORK_CONSOLE_USER_PROFILE(4), // The profile is the active console user's per user profile
    WLAN_AVAILABLE_NETWORK_INTERWORKING_SUPPORTED(8), // Interworking is supported
    WLAN_AVAILABLE_NETWORK_HOTSPOT2_ENABLED(16), // Hotspot2 is enabled
    WLAN_AVAILABLE_NETWORK_ANQP_SUPPORTED(32), // ANQP is supported
    WLAN_AVAILABLE_NETWORK_HOTSPOT2_DOMAIN(64), // Domain network
    WLAN_AVAILABLE_NETWORK_HOTSPOT2_ROAMING(128), // Roaming network
    WLAN_AVAILABLE_NETWORK_AUTO_CONNECT_FAILED(256); // This network failed to connect

    private long val;

    NetworkFlagsEnum(long val) {
        this.val = val;
    }

    public long getVal() {
        return val;
    }

    public static NetworkFlagsEnum getFromVal(long val)
    {
        NetworkFlagsEnum networkFlagsEnum = null;
        if (val == 1) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_CONNECTED;
        } else if (val == 2) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_HAS_PROFILE;
        } else if (val == 4) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_CONSOLE_USER_PROFILE;
        } else if (val == 8) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_INTERWORKING_SUPPORTED;
        } else if (val == 16) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_HOTSPOT2_ENABLED;
        } else if (val == 32) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_ANQP_SUPPORTED;
        } else if (val == 64) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_HOTSPOT2_DOMAIN;
        } else if (val == 128) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_HOTSPOT2_ROAMING;
        } else if (val == 256) {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK_AUTO_CONNECT_FAILED;
        } else {
            networkFlagsEnum = NetworkFlagsEnum.WLAN_AVAILABLE_NETWORK;
        }
        return networkFlagsEnum;
    }
}
