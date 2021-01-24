package com.example.network.tweaker.util;

public class WiFiNetwork {

    private String ssid;
    private String rssi;
    private String flags;

    public WiFiNetwork(String ssid, String rssi, String flags) {
        this.ssid = ssid;
        this.rssi = rssi;
        this.flags = flags;
    }

    public String getSsid() {
        return ssid;
    }

    public void setSsid(String ssid) {
        this.ssid = ssid;
    }

    public String getRssi() {
        return rssi;
    }

    public void setRssi(String rssi) {
        this.rssi = rssi;
    }

    public String getFlags() {
        return flags;
    }

    public void setFlags(String flags) {
        this.flags = flags;
    }

    @Override
    public String toString() {
        return ssid;
    }
}
