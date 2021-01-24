package com.example.network.tweaker.controller;

import com.example.network.tweaker.NetworkTweaker;
import com.example.network.tweaker.NetworkTweaker.NetworkInfo;
import com.example.network.tweaker.NetworkTweaker.WiFiInfo;
import com.example.network.tweaker.util.DialogManager;
import com.example.network.tweaker.util.NetworkCommandEnum;
import com.example.network.tweaker.util.NetworkFlagsEnum;
import com.example.network.tweaker.util.WiFiNetwork;
import java.util.ArrayList;
import javafx.concurrent.Task;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Button;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.stage.Window;

public class AvailableNetworksController {

    @FXML
    public DialogPane availableNetworksPane;
    @FXML
    public TableView<WiFiNetwork> tblAvailableNetworks;
    @FXML
    public TableColumn<WiFiNetwork, String> networkCol;
    @FXML
    public TableColumn<WiFiNetwork, String> signalCol;
    @FXML
    public TableColumn<WiFiNetwork, String> statusCol;
    @FXML
    public Button btnCancel;
    @FXML
    public Button btnConnect;

    private Dialog dialog;
    private NetworkTweaker networkTweaker;
    private NetworkTweaker.NetworkInfo networkInfo;

    public void initialize() {
        dialog = DialogManager.createAvailableNetworksDialog(availableNetworksPane);
        networkCol.setCellValueFactory(new PropertyValueFactory<>("ssid"));
        signalCol.setCellValueFactory(new PropertyValueFactory<>("rssi"));
        statusCol.setCellValueFactory(new PropertyValueFactory<>("flags"));
        initializeListeners();
    }

    public void showAvailableNetworks() {
        dialog.show();
    }

    private void initializeListeners() {
        btnCancel.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent e) {
                Window window = dialog.getDialogPane().getScene().getWindow();
                window.hide();
            }
        });
        btnConnect.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent e) {
                if (tblAvailableNetworks.getSelectionModel().isEmpty()) {
                    DialogManager.showAlertDialog(AlertType.WARNING, "Network Selection is required.");
                } else {
                    WiFiNetwork wiFiNetwork = tblAvailableNetworks.getSelectionModel().getSelectedItem();
                    Dialog waitingDialog = DialogManager.createWaitingDialog();
                    Task<Boolean> task = new Task<Boolean>() {
                        @Override public Boolean call() throws InterruptedException {
                            networkTweaker.setWifiConnectParameters(wiFiNetwork.getSsid());
                            networkTweaker.setCommandVal(NetworkCommandEnum.WifiConnect);
                            Thread.sleep(5000);
                            networkTweaker.setWifiGetAvailableNetworksParameters(networkInfo.getGuid());
                            networkTweaker.setCommandVal(NetworkCommandEnum.WifiGetAvailableNetworks);
                            return true;
                        }
                    };
                    task.setOnRunning(event -> waitingDialog.show());
                    task.setOnSucceeded(event -> {
                        waitingDialog.getDialogPane().getScene().getWindow().hide();
                    });
                    new Thread(task).start();
                }
            }
        });
        Window window = dialog.getDialogPane().getScene().getWindow();
        window.setOnCloseRequest(event -> window.hide());
    }

    public void updateAvailableNetworks(ArrayList<WiFiInfo> wiFiInfoList) {
        tblAvailableNetworks.getItems().clear();
        wiFiInfoList.forEach(wiFiInfo -> {
            int rssi = wiFiInfo.getRssi();
            String strRssi = rssi + " dBm";
            NetworkFlagsEnum networkFlagsEnum = NetworkFlagsEnum.getFromVal(wiFiInfo.getFlags());
            WiFiNetwork wiFiNetwork = new WiFiNetwork(wiFiInfo.getSsid(), strRssi, networkFlagsEnum.name());
            tblAvailableNetworks.getItems().add(wiFiNetwork);
        });
        tblAvailableNetworks.refresh();
    }

    public void setNetworkTweaker(NetworkTweaker networkTweaker) {
        this.networkTweaker = networkTweaker;
    }

    public void setNetworkInfo(NetworkInfo networkInfo) {
        this.networkInfo = networkInfo;
    }
}
