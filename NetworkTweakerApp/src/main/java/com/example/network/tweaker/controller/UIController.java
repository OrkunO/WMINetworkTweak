package com.example.network.tweaker.controller;

import com.example.network.tweaker.NetworkTweaker;
import com.example.network.tweaker.NetworkTweaker.NetworkInfo;
import com.example.network.tweaker.util.DialogManager;
import com.example.network.tweaker.util.IPTextField;
import com.example.network.tweaker.util.NetworkCommandEnum;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.*;

import java.util.ArrayList;

public class UIController {

    @FXML
    public ComboBox<NetworkTweaker.NetworkInfo> cmbNetworkAdapters;
    @FXML
    public CheckBox chkNetworkAdapterEnable;
    @FXML
    public RadioButton rbManualConfiguration;
    @FXML
    public RadioButton rbEnableDHCP;
    @FXML
    public Label lblIPAddress;
    @FXML
    public Label lblSubnetMask;
    @FXML
    public Label lblGateway;
    @FXML
    public IPTextField txtIPAddress;
    @FXML
    public IPTextField txtSubnetMask;
    @FXML
    public IPTextField txtGateway;
    @FXML
    public Button btnSave;
    @FXML
    public Button btnRefresh;
    @FXML
    public Button btnShowAvailableNetworks;

    private NetworkTweaker networkTweaker;
    private NetworkTweaker.NetworkInfo lastSelectedItem;
    private AvailableNetworksController availableNetworksController;

    @FXML
    public void initialize() {
        initializeListeners();

        final ToggleGroup group = new ToggleGroup();
        rbManualConfiguration.setToggleGroup(group);
        rbEnableDHCP.setToggleGroup(group);
    }

    private void initializeListeners() {
        chkNetworkAdapterEnable.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                boolean networkAdapterEnableSelected = chkNetworkAdapterEnable.isSelected();
                NetworkTweaker.NetworkInfo networkInfo = cmbNetworkAdapters.getSelectionModel().getSelectedItem();
                networkTweaker.setTweakNetworkParameters(networkAdapterEnableSelected, networkInfo.getDeviceId());
                networkTweaker.setCommandVal(NetworkCommandEnum.TweakNetwork);
                networkTweaker.setCommandVal(NetworkCommandEnum.GetNetworkInfo);
                DialogManager.showAlertDialog(Alert.AlertType.INFORMATION, "Selected Network Adapter is "
                        + (networkAdapterEnableSelected ? "Enabled" : "Disabled"));
            }
        });
        btnRefresh.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent e) {
                networkTweaker.setCommandVal(NetworkCommandEnum.GetNetworkInfo);
            }
        });
        btnSave.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent e) {
                NetworkTweaker.NetworkInfo networkInfo = cmbNetworkAdapters.getSelectionModel().getSelectedItem();
                if (rbManualConfiguration.isSelected()) {
                    if (txtIPAddress.validate() && txtSubnetMask.validate() && txtGateway.validate()) {
                        int deviceIndex = networkInfo.getDeviceIndex();
                        String ipAddress = txtIPAddress.getText();
                        String subnetMask = txtSubnetMask.getText();
                        String gateway = txtGateway.getText();
                        networkTweaker.setEnableStaticParameters(deviceIndex, ipAddress, subnetMask, gateway);
                        networkTweaker.setCommandVal(NetworkCommandEnum.EnableStatic);
//                        networkTweaker.setCommandVal(NetworkCommandEnum.GetNetworkInfo);
                    } else {
                        DialogManager.showAlertDialog(Alert.AlertType.WARNING, "One or more input fields have validation errors.");
                    }
                } else {
                    networkTweaker.setEnableDHCPParameters(networkInfo.getDeviceIndex());
                    networkTweaker.setCommandVal(NetworkCommandEnum.EnableDHCP);
                    networkTweaker.setCommandVal(NetworkCommandEnum.GetNetworkInfo);
                }
            }
        });
        btnShowAvailableNetworks.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent e) {
                NetworkTweaker.NetworkInfo networkInfo = cmbNetworkAdapters.getSelectionModel().getSelectedItem();
                networkTweaker.setWifiGetAvailableNetworksParameters(networkInfo.getGuid());
                networkTweaker.setCommandVal(NetworkCommandEnum.WifiGetAvailableNetworks);
                availableNetworksController.setNetworkInfo(networkInfo);
                availableNetworksController.showAvailableNetworks();
            }
        });
        rbEnableDHCP.selectedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if (rbEnableDHCP.isSelected()) {
                    disableIP4Fields(true);
                    lblGateway.setText("DHCP Server");
                }
            }
        });
        rbManualConfiguration.selectedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if (rbManualConfiguration.isSelected()) {
                    disableIP4Fields(false);
                    lblGateway.setText("Gateway");
                }
            }
        });
        cmbNetworkAdapters.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                if (cmbNetworkAdapters.getItems().size() > 0) {
                    NetworkTweaker.NetworkInfo networkInfo = cmbNetworkAdapters.getSelectionModel().getSelectedItem();
                    lastSelectedItem = networkInfo;

                    rbEnableDHCP.setDisable(!networkInfo.isNetEnabled());
                    rbManualConfiguration.setDisable(!networkInfo.isNetEnabled());
                    disableIP4Fields(!networkInfo.isNetEnabled());

                    if (networkInfo.isDhcpEnabled()) {
                        rbEnableDHCP.setSelected(true);
                    } else {
                        rbManualConfiguration.setSelected(true);
                    }
                    if (networkInfo.getNetConnectionId().contains("Wi-Fi"))
                    {
                        btnShowAvailableNetworks.setDisable(false);
                    } else {
                        btnShowAvailableNetworks.setDisable(true);
                    }
                    chkNetworkAdapterEnable.setSelected(networkInfo.isNetEnabled());
                    txtIPAddress.setText(networkInfo.getIpAddress());
                    txtSubnetMask.setText(networkInfo.getSubnetMask());
                    if (rbEnableDHCP.isSelected()) {
                        txtGateway.setText(networkInfo.getDhcpServer());
                    } else {
                        txtGateway.setText(networkInfo.getGateway());
                    }
                }
            }
        });
    }

    private void disableIP4Fields(boolean disable) {
        txtIPAddress.setDisable(disable);
        txtSubnetMask.setDisable(disable);
        txtGateway.setDisable(disable);

        lblIPAddress.setDisable(disable);
        lblSubnetMask.setDisable(disable);
        lblGateway.setDisable(disable);
    }

    public void updateNetworkAdapterData(ArrayList<NetworkTweaker.NetworkInfo> networkInfoList) {
        cmbNetworkAdapters.getItems().clear();
        networkInfoList.forEach(networkInfo -> {
            if (!networkInfo.getNetConnectionId().equals("-1")) {
                cmbNetworkAdapters.getItems().add(networkInfo);
            }
        });
        if (lastSelectedItem == null) {
            cmbNetworkAdapters.getSelectionModel().selectFirst();
        } else {
            if (cmbNetworkAdapters.getItems().contains(lastSelectedItem)) {
                cmbNetworkAdapters.getSelectionModel().select(lastSelectedItem);
            }
        }
    }

    public void setNetworkTweaker(NetworkTweaker networkTweaker) {
        this.networkTweaker = networkTweaker;
    }

    public void setAvailableNetworksController(AvailableNetworksController availableNetworksController) {
        this.availableNetworksController = availableNetworksController;
    }
}
