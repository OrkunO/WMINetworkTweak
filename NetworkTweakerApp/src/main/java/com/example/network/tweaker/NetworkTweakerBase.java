package com.example.network.tweaker;

import com.example.network.tweaker.NetworkTweaker.WiFiInfo;
import com.example.network.tweaker.controller.AvailableNetworksController;
import com.example.network.tweaker.controller.UIController;
import com.example.network.tweaker.util.NetworkCommandEnum;
import com.example.network.tweaker.util.ResourceHelper;
import java.net.URL;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Rectangle2D;
import javafx.scene.Scene;
import javafx.scene.layout.VBox;
import javafx.stage.Screen;
import javafx.stage.Stage;

public class NetworkTweakerBase extends Application {

    public native ArrayList<NetworkTweaker.NetworkInfo> getNetworkInfo();

    public native boolean initialize();

    public native boolean tweakNetwork(boolean enable, String deviceId);

    public native boolean enableStaticWithGateway(int deviceIndex, String ipAddress, String subnetMask, String gateway);

    public native boolean enableDHCP(int deviceIndex);

    public native boolean wifiInitialize();

    public native ArrayList<NetworkTweaker.WiFiInfo> wifiGetAvailableNetworkList(String interfaceGUID);

    public native boolean wifiConnect(String wifiSSID);

    private UIController uiController;
    private AvailableNetworksController availableNetworksController;
    private ExecutorService executorService = Executors.newSingleThreadExecutor();
    private NetworkTweaker networkTweaker;

    // --- Main method to test our native library
    public static void main(String[] args) {
        System.loadLibrary("tweaknetworkjni");

        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        networkTweaker = new NetworkTweaker(NetworkTweakerBase.this);
        executorService.execute(networkTweaker);
        executorService.shutdown();

        networkTweaker.setCommandVal(NetworkCommandEnum.Initialize);
        networkTweaker.setCommandVal(NetworkCommandEnum.WifiInitialize);

        URL fxmlUIUrl = ResourceHelper.getInstance().getFileURL("ui.fxml");
        URL fxmlAvailableNetworksURL = ResourceHelper.getInstance().getFileURL("availableNetworks.fxml");

        // Load the FXML document
        FXMLLoader fxmlUILoader = new FXMLLoader(fxmlUIUrl);
        VBox root = fxmlUILoader.load();
        FXMLLoader fxmlAvailableNetworksLoader = new FXMLLoader(fxmlAvailableNetworksURL);
        fxmlAvailableNetworksLoader.load();
        availableNetworksController = (AvailableNetworksController) fxmlAvailableNetworksLoader.getController();
        availableNetworksController.setNetworkTweaker(networkTweaker);
        uiController = (UIController) fxmlUILoader.getController();
        uiController.setNetworkTweaker(networkTweaker);
        uiController.setAvailableNetworksController(availableNetworksController);
        Scene scene = new Scene(root);
        primaryStage.setScene(scene);
        primaryStage.setTitle("Network Tweaker");
        primaryStage.show();

        Rectangle2D primScreenBounds = Screen.getPrimary().getVisualBounds();
        primaryStage.setX((primScreenBounds.getWidth() - primaryStage.getWidth()) / 2);
        primaryStage.setY((primScreenBounds.getHeight() - primaryStage.getHeight()) / 2);

        //pausableThreadPoolExecutor.resume();
        //networkTweaker.setTweakNetworkParameters(false, "4");
        //networkTweaker.setCommandVal(NetworkCommandEnum.TweakNetwork);

        networkTweaker.setCommandVal(NetworkCommandEnum.GetNetworkInfo);
        //networkTweakerThread.start();
//        networkTweaker.tweakNetwork(true, "4");
    }

    public void networkInfoReceived(ArrayList<NetworkTweaker.NetworkInfo> networkInfoList) {
        for (NetworkTweaker.NetworkInfo networkInfo : networkInfoList) {
            System.out.println("Device ID = " + networkInfo.getDeviceId()
                    + " - Index = " + networkInfo.getDeviceIndex()
                    + " - Net Connection ID = " + networkInfo.getNetConnectionId()
                    + " - GUID = " + networkInfo.getGuid()
                    + " - Net Enabled = " + networkInfo.isNetEnabled()
                    + " - IP Address = " + networkInfo.getIpAddress()
                    + " - Subnet Mask = " + networkInfo.getSubnetMask()
                    + " - Gateway = " + networkInfo.getGateway()
                    + " - DHCP Server = " + networkInfo.getDhcpServer()
                    + " - DHCP Enabled = " + networkInfo.isDhcpEnabled());
        }
        Platform.runLater(new Runnable() {
            @Override
            public void run() {
                uiController.updateNetworkAdapterData(networkInfoList);
            }
        });

    }

    public void wifiAvailableNetworksReceived(ArrayList<NetworkTweaker.WiFiInfo> wifiGetAvailableNetworkList) {
        Platform.runLater(new Runnable() {
            @Override
            public void run() {
                availableNetworksController.updateAvailableNetworks(wifiGetAvailableNetworkList);
            }
        });
    }
}