package com.example.network.tweaker.util;

import java.util.Optional;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;

public class DialogManager {

    public static void showAlertDialog(Alert.AlertType alertType, String text) {
        new AlertDialog(alertType, text);
    }

    public static boolean showConfirmationDialog(String text) {
        AlertDialog alertDialog = new AlertDialog(Alert.AlertType.CONFIRMATION, text);
        Optional<ButtonType> result = alertDialog.showAndWait();
        if ((result.isPresent()) && (result.get() == ButtonType.OK)) {
            return true;
        }
        return false;
    }

    public static Dialog createWaitingDialog()
    {
        Dialog dialog = new Dialog();
        dialog.setHeaderText("Information");
        dialog.setContentText("Operation In Progress ...");
        return dialog;
    }

    public static Dialog createAvailableNetworksDialog(DialogPane dialogPane) {
        Dialog dialog = new Dialog();
        dialog.setTitle("Available Networks");
        dialog.setDialogPane(dialogPane);
        return dialog;
    }
}
