package com.example.network.tweaker.util;

import java.io.File;
import java.io.InputStream;
import java.net.URL;

public class ResourceHelper {


    private static ResourceHelper resourceHelper;

    private ResourceHelper()
    {

    }

    public static ResourceHelper getInstance()
    {
        if (resourceHelper == null)
        {
            resourceHelper = new ResourceHelper();
        }
        return resourceHelper;
    }

    public URL getFileURL(String fileName)
    {
        return ResourceHelper.class.getClassLoader().getResource(fileName);
    }

    public InputStream getFileInputStream(String fileName)
    {
        return ResourceHelper.class.getClassLoader().getResourceAsStream(fileName);
    }



}
