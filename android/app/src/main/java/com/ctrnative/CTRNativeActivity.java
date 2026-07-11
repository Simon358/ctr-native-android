package com.ctrnative;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.util.Log;

import org.libsdl.app.SDLActivity;

import java.io.File;

public class CTRNativeActivity extends SDLActivity {
    private static final int PICK_DIRECTORY_REQUEST = 1001;
    private static final String PREFS_NAME = "CTRNativePrefs";
    private static final String KEY_ASSET_PATH = "assetPath";

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL3",
            "ctr_native"
        };
    }

    public static void pickDirectory() {
        Activity activity = (Activity) SDLActivity.getContext();
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
        activity.startActivityForResult(intent, PICK_DIRECTORY_REQUEST);
    }

    public static String getStoredAssetPath() {
        Activity activity = (Activity) SDLActivity.getContext();
        SharedPreferences prefs = activity.getSharedPreferences(PREFS_NAME, MODE_PRIVATE);
        return prefs.getString(KEY_ASSET_PATH, null);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PICK_DIRECTORY_REQUEST && resultCode == RESULT_OK) {
            if (data != null) {
                Uri uri = data.getData();
                getContentResolver().takePersistableUriPermission(uri, 
                        Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
                
                String path = getPathFromUri(uri);
                if (path != null) {
                    SharedPreferences.Editor editor = getSharedPreferences(PREFS_NAME, MODE_PRIVATE).edit();
                    editor.putString(KEY_ASSET_PATH, path);
                    editor.apply();
                    Log.d("CTRNative", "Stored asset path: " + path);
                }
            }
        }
    }

    private String getPathFromUri(Uri uri) {
        if (uri == null) return null;
        String path = null;
        if ("com.android.externalstorage.documents".equals(uri.getHost())) {
            final String docId = DocumentsContract.getTreeDocumentId(uri);
            final String[] split = docId.split(":");
            final String type = split[0];
            if ("primary".equalsIgnoreCase(type)) {
                path = Environment.getExternalStorageDirectory() + "/" + split[1];
            } else {
                // Secondary SD cards
                path = "/storage/" + type + "/" + split[1];
            }
        }
        return path;
    }
}
