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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        checkStoragePermission();
    }

    private void checkStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                try {
                    Intent intent = new Intent(android.provider.Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                    intent.addCategory("android.intent.category.DEFAULT");
                    intent.setData(Uri.parse(String.format("package:%s", getApplicationContext().getPackageName())));
                    startActivity(intent);
                } catch (Exception e) {
                    Intent intent = new Intent();
                    intent.setAction(android.provider.Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                    startActivity(intent);
                }
            }
        }
    }

    private static boolean pickerActive = false;

    public static void pickDirectory() {
        Activity activity = (Activity) SDLActivity.getContext();
        pickerActive = true;
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
        activity.startActivityForResult(intent, PICK_DIRECTORY_REQUEST);
    }

    public static boolean isPickerActive() {
        return pickerActive;
    }

    public static String getStoredAssetPath() {
        Activity activity = (Activity) SDLActivity.getContext();
        SharedPreferences prefs = activity.getSharedPreferences(PREFS_NAME, MODE_PRIVATE);
        String path = prefs.getString(KEY_ASSET_PATH, null);
        Log.d("CTRNative", "getStoredAssetPath: " + path);
        return path;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PICK_DIRECTORY_REQUEST) {
            pickerActive = false;
            if (resultCode == RESULT_OK && data != null) {
                Uri uri = data.getData();
                try {
                    getContentResolver().takePersistableUriPermission(uri,
                            Intent.FLAG_GRANT_READ_URI_PERMISSION);

                    String path = getPathFromUri(uri);
                    if (path != null) {
                        SharedPreferences.Editor editor = getSharedPreferences(PREFS_NAME, MODE_PRIVATE).edit();
                        editor.putString(KEY_ASSET_PATH, path);
                        editor.apply();
                        Log.d("CTRNative", "Stored asset path: " + path);
                        
                        // Show a message to the user
                        android.widget.Toast.makeText(this, "Assets folder saved. Please restart the app.", android.widget.Toast.LENGTH_LONG).show();
                    } else {
                        Log.e("CTRNative", "Failed to resolve path from URI: " + uri.toString());
                    }
                } catch (Exception e) {
                    Log.e("CTRNative", "Error taking persistable permission: " + e.getMessage());
                }
            }
        }
    }

    private String getPathFromUri(Uri uri) {
        if (uri == null) return null;
        
        String path = null;
        String host = uri.getHost();
        String docId = null;
        
        try {
            if ("com.android.externalstorage.documents".equals(host)) {
                docId = DocumentsContract.getTreeDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];
                if ("primary".equalsIgnoreCase(type)) {
                    path = Environment.getExternalStorageDirectory() + "/" + (split.length > 1 ? split[1] : "");
                } else {
                    // Secondary SD cards
                    path = "/storage/" + type + "/" + (split.length > 1 ? split[1] : "");
                }
            } else if ("com.android.providers.downloads.documents".equals(host)) {
                docId = DocumentsContract.getTreeDocumentId(uri);
                if (docId.startsWith("raw:")) {
                    path = docId.substring(4);
                } else {
                    path = Environment.getExternalStorageDirectory() + "/Download/" + docId;
                }
            }
        } catch (Exception e) {
            Log.e("CTRNative", "Error in getPathFromUri: " + e.getMessage());
        }
        
        if (path != null) {
            path = path.replace("//", "/");
            if (path.endsWith("/")) {
                path = path.substring(0, path.length() - 1);
            }
        }

        return path;
    }
}
