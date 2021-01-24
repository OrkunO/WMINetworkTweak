package com.example.network.tweaker.util;

import javafx.scene.control.Alert;

public class AlertDialog extends Alert {
    public AlertDialog(AlertType alertType, String text) {
        super(alertType);
        switch (alertType)
        {
            case CONFIRMATION:
                setTitle("Confirmation");
                setContentText(text);
                break;
            case NONE:
                break;
            case INFORMATION:
                setTitle("Information");
                setHeaderText(text);
                showAndWait();
                break;
            case ERROR:
                setTitle("Error");
                setHeaderText(text);
                showAndWait();
                break;
            case WARNING:
                setTitle("Warning");
                setHeaderText(text);
                showAndWait();
                break;
        }
    }
}
