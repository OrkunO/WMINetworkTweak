package com.example.network.tweaker;

import com.example.network.tweaker.util.NetworkCommand;
import com.example.network.tweaker.util.NetworkCommandEnum;
import java.util.Objects;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class NetworkTweaker implements Runnable {

    private NetworkTweakerBase networkTweakerBase;

    private boolean isAppRunning;
    private boolean isPaused;
    private ReentrantLock pauseLock = new ReentrantLock();
    private Condition unpaused = pauseLock.newCondition();
    private Object operationInProgress = new Object();

    private AtomicInteger commandCount;
    private ConcurrentLinkedQueue<NetworkCommand> networkCommandLQueue;
    private AtomicBoolean enableNetwork;
    private AtomicInteger deviceIndex;
    private AtomicReference<String> deviceId;
    private AtomicReference<String> ipAddress;
    private AtomicReference<String> subnetMask;
    private AtomicReference<String> gateway;
    private AtomicReference<String> wifiSSID;
    private AtomicReference<String> interfeceGUID;

    public NetworkTweaker(NetworkTweakerBase networkTweakerBase) {
        this.networkTweakerBase = networkTweakerBase;
        networkCommandLQueue = new ConcurrentLinkedQueue<>();
        operationInProgress = false;
        isAppRunning = true;
        commandCount = new AtomicInteger(0);
        setToDefault();
    }

    private void setToDefault() {
        deviceIndex = new AtomicInteger(-1);
        enableNetwork = new AtomicBoolean(false);
        deviceId = new AtomicReference<>("-1");
        ipAddress = new AtomicReference<>("0.0.0.0");
        subnetMask = new AtomicReference<>("0.0.0.0");
        gateway = new AtomicReference<>("0.0.0.0");
        wifiSSID = new AtomicReference<>("-1");
        interfeceGUID = new AtomicReference<>("{}");
    }


    public void setCommandVal(NetworkCommandEnum networkCommandEnum) {
        synchronized (operationInProgress) {
            System.out.println("Set Command = " + networkCommandEnum.name());
            NetworkInfo networkInfo = new NetworkInfo();
            WiFiInfo wiFiInfo = new WiFiInfo();
            switch (networkCommandEnum) {
                case Initialize:
                    break;
                case GetNetworkInfo:
                    break;
                case TweakNetwork:
                    networkInfo.setDeviceId(deviceId.get());
                    networkInfo.setNetEnabled(enableNetwork.get());
                    break;
                case EnableStatic:
                    networkInfo.setDeviceIndex(deviceIndex.get());
                    networkInfo.setIpAddress(ipAddress.get());
                    networkInfo.setSubnetMask(subnetMask.get());
                    networkInfo.setGateway(gateway.get());
                    break;
                case EnableDHCP:
                    networkInfo.setDeviceIndex(deviceIndex.get());
                    break;
                case WifiInitialize:
                    break;
                case WifiGetAvailableNetworks:
                    networkInfo.setGuid(interfeceGUID.get());
                    break;
                case WifiConnect:
                    wiFiInfo.setSsid(wifiSSID.get());
                    break;
                case No_Val:
                    break;
            }
            NetworkCommand networkCommand = new NetworkCommand(commandCount.get(), networkInfo, wiFiInfo, networkCommandEnum);
            commandCount.set(commandCount.get() + 1);
            networkCommandLQueue.add(networkCommand);
            resume();
        }
    }

    @Override
    public void run() {
        while (isAppRunning) {
            System.out.println("Network Tweaker is running...");
            if (isPaused) {
                pauseLock.lock();
                try {
                    while (isPaused) {
                        System.out.println("Network Tweaker paused...");
                        try {
                            unpaused.await();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                } finally {
                    pauseLock.unlock();
                }
            }
            networkCommandLQueue.forEach(networkCommand -> {
                switch (networkCommand.getNetworkCommandEnum()) {
                    case Initialize:
                        networkTweakerBase.initialize();
                        break;
                    case GetNetworkInfo:
                        networkTweakerBase.networkInfoReceived(networkTweakerBase.getNetworkInfo());
                        break;
                    case TweakNetwork:
                        networkTweakerBase.tweakNetwork(networkCommand.getNetworkInfo().isNetEnabled(),
                                networkCommand.getNetworkInfo().getDeviceId());
                        break;
                    case EnableStatic:
                        networkTweakerBase.enableStaticWithGateway(networkCommand.getNetworkInfo().getDeviceIndex(),
                                networkCommand.getNetworkInfo().getIpAddress(),
                                networkCommand.getNetworkInfo().getSubnetMask(),
                                networkCommand.getNetworkInfo().getGateway());
                        break;
                    case EnableDHCP:
                        networkTweakerBase.enableDHCP(networkCommand.getNetworkInfo().getDeviceIndex());
                        break;
                    case WifiInitialize:
                        networkTweakerBase.wifiInitialize();
                        break;
                    case WifiGetAvailableNetworks:
                        networkTweakerBase.wifiAvailableNetworksReceived(
                                networkTweakerBase.wifiGetAvailableNetworkList(networkCommand.getNetworkInfo().getGuid()));
                        break;
                    case WifiConnect:
                        networkTweakerBase.wifiConnect(networkCommand.getWiFiInfo().getSsid());
                        break;
                    case No_Val:
                        break;
                }
                networkCommandLQueue.remove();
            });
            setToDefault();
            if (networkCommandLQueue.size() == 0) {
                pause();
            }
        }
    }

    public void pause() {
        pauseLock.lock();
        try {
            isPaused = true;
        } finally {
            pauseLock.unlock();
        }
    }

    public void resume() {
        pauseLock.lock();
        try {
            if (isPaused) {
                isPaused = false;
                unpaused.signalAll();
            }
        } finally {
            pauseLock.unlock();
        }
    }

    public void setTweakNetworkParameters(boolean enableNetwork, String deviceId) {
        this.enableNetwork.set(enableNetwork);
        this.deviceId.set(deviceId);
    }

    public void setEnableStaticParameters(int deviceIndex, String ipAddress, String subnetMask, String gateway) {
        this.deviceIndex.set(deviceIndex);
        this.ipAddress.set(ipAddress);
        this.subnetMask.set(subnetMask);
        this.gateway.set(gateway);
    }

    public void setEnableDHCPParameters(int deviceIndex) {
        this.deviceIndex.set(deviceIndex);
    }

    public void setWifiGetAvailableNetworksParameters(String interfaceGUID) {
        this.interfeceGUID.set(interfaceGUID);
    }

    public void setWifiConnectParameters(String wifiSSID) {
        this.wifiSSID.set(wifiSSID);
    }

    public class NetworkInfo {

        private String deviceId = "-1";
        private int deviceIndex = -1;
        private String netConnectionId = "-1";
        private String guid = "-1";
        private boolean netEnabled = false;
        private boolean dhcpEnabled = false;
        private String ipAddress = "0.0.0.0";
        private String subnetMask = "0.0.0.0";
        ;
        private String gateway = "0.0.0.0";
        ;
        private String dhcpServer = "0.0.0.0";
        ;

        public String getDeviceId() {
            return deviceId;
        }

        public void setDeviceId(String deviceId) {
            this.deviceId = deviceId;
        }

        public int getDeviceIndex() {
            return deviceIndex;
        }

        public void setDeviceIndex(int deviceIndex) {
            this.deviceIndex = deviceIndex;
        }

        public String getNetConnectionId() {
            return netConnectionId;
        }

        public void setNetConnectionId(String netConnectionId) {
            this.netConnectionId = netConnectionId;
        }

        public String getGuid() {
            return guid;
        }

        public void setGuid(String guid) {
            this.guid = guid;
        }

        public boolean isNetEnabled() {
            return netEnabled;
        }

        public void setNetEnabled(boolean netEnabled) {
            this.netEnabled = netEnabled;
        }

        public boolean isDhcpEnabled() {
            return dhcpEnabled;
        }

        public void setDhcpEnabled(boolean dhcpEnabled) {
            this.dhcpEnabled = dhcpEnabled;
        }

        public String getIpAddress() {
            return ipAddress;
        }

        public void setIpAddress(String ipAddress) {
            this.ipAddress = ipAddress;
        }

        public String getSubnetMask() {
            return subnetMask;
        }

        public void setSubnetMask(String subnetMask) {
            this.subnetMask = subnetMask;
        }

        public String getGateway() {
            return gateway;
        }

        public void setGateway(String gateway) {
            this.gateway = gateway;
        }

        public String getDhcpServer() {
            return dhcpServer;
        }

        public void setDhcpServer(String dhcpServer) {
            this.dhcpServer = dhcpServer;
        }

        @Override
        public String toString() {
            return netConnectionId;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            NetworkInfo that = (NetworkInfo) o;
            return deviceIndex == that.deviceIndex &&
                    netEnabled == that.netEnabled &&
                    dhcpEnabled == that.dhcpEnabled &&
                    Objects.equals(deviceId, that.deviceId) &&
                    Objects.equals(netConnectionId, that.netConnectionId) &&
                    Objects.equals(ipAddress, that.ipAddress) &&
                    Objects.equals(subnetMask, that.subnetMask) &&
                    Objects.equals(gateway, that.gateway) &&
                    Objects.equals(dhcpServer, that.dhcpServer);
        }

        @Override
        public int hashCode() {
            return Objects.hash(deviceId, deviceIndex, netConnectionId, netEnabled, dhcpEnabled, ipAddress, subnetMask, gateway, dhcpServer);
        }
    }

    public class WiFiInfo {

        private String ssid;
        private int rssi;
        private long flags;

        public String getSsid() {
            return ssid;
        }

        public void setSsid(String ssid) {
            this.ssid = ssid;
        }

        public int getRssi() {
            return rssi;
        }

        public void setRssi(int rssi) {
            this.rssi = rssi;
        }

        public long getFlags() {
            return flags;
        }

        public void setFlags(long flags) {
            this.flags = flags;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            WiFiInfo wiFiInfo = (WiFiInfo) o;
            return rssi == wiFiInfo.rssi &&
                    flags == wiFiInfo.flags &&
                    Objects.equals(ssid, wiFiInfo.ssid);
        }

        @Override
        public int hashCode() {
            return Objects.hash(ssid, rssi, flags);
        }
    }

}
