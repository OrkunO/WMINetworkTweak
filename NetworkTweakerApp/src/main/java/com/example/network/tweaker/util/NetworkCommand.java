package com.example.network.tweaker.util;

import com.example.network.tweaker.NetworkTweaker;

import com.example.network.tweaker.NetworkTweaker.NetworkInfo;
import com.example.network.tweaker.NetworkTweaker.WiFiInfo;
import java.util.Objects;

public class NetworkCommand {

    private int commandCount;
    private NetworkTweaker.NetworkInfo networkInfo;
    private NetworkTweaker.WiFiInfo wiFiInfo;
    private NetworkCommandEnum networkCommandEnum;

    public NetworkCommand(int commandCount, NetworkInfo networkInfo, WiFiInfo wiFiInfo,
            NetworkCommandEnum networkCommandEnum) {
        this.commandCount = commandCount;
        this.networkInfo = networkInfo;
        this.wiFiInfo = wiFiInfo;
        this.networkCommandEnum = networkCommandEnum;
    }

    public int getCommandCount() {
        return commandCount;
    }

    public void setCommandCount(int commandCount) {
        this.commandCount = commandCount;
    }

    public NetworkTweaker.NetworkInfo getNetworkInfo() {
        return networkInfo;
    }

    public void setNetworkInfo(NetworkTweaker.NetworkInfo networkInfo) {
        this.networkInfo = networkInfo;
    }

    public WiFiInfo getWiFiInfo() {
        return wiFiInfo;
    }

    public void setWiFiInfo(WiFiInfo wiFiInfo) {
        this.wiFiInfo = wiFiInfo;
    }

    public NetworkCommandEnum getNetworkCommandEnum() {
        return networkCommandEnum;
    }

    public void setNetworkCommandEnum(NetworkCommandEnum networkCommandEnum) {
        this.networkCommandEnum = networkCommandEnum;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        NetworkCommand that = (NetworkCommand) o;
        return commandCount == that.commandCount &&
                Objects.equals(networkInfo, that.networkInfo) &&
                Objects.equals(wiFiInfo, that.wiFiInfo) &&
                networkCommandEnum == that.networkCommandEnum;
    }

    @Override
    public int hashCode() {
        return Objects.hash(commandCount, networkInfo, wiFiInfo, networkCommandEnum);
    }
}
