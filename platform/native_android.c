#ifdef __ANDROID__
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <jni.h>
#include "platform/native_log.h"

void Platform_Android_PickFolder(void) {
    JNIEnv* env = (JNIEnv*)SDL_GetAndroidJNIEnv();
    jobject activity = (jobject)SDL_GetAndroidActivity();
    jclass clazz = (*env)->GetObjectClass(env, activity);
    jmethodID methodId = (*env)->GetStaticMethodID(env, clazz, "pickDirectory", "()V");

    if (methodId) {
        (*env)->CallStaticVoidMethod(env, clazz, methodId);
    } else {
        Platform_LogError("[CTR Android] Failed to find pickDirectory method\n");
    }

    (*env)->DeleteLocalRef(env, activity);
    (*env)->DeleteLocalRef(env, clazz);
}

char* Platform_Android_GetStoredPath(void) {
    JNIEnv* env = (JNIEnv*)SDL_GetAndroidJNIEnv();
    jobject activity = (jobject)SDL_GetAndroidActivity();
    jclass clazz = (*env)->GetObjectClass(env, activity);
    jmethodID methodId = (*env)->GetStaticMethodID(env, clazz, "getStoredAssetPath", "()Ljava/lang/String;");

    char* result = NULL;
    if (methodId) {
        jstring jPath = (jstring)(*env)->CallStaticObjectMethod(env, clazz, methodId);
        if (jPath) {
            const char* pathChars = (*env)->GetStringUTFChars(env, jPath, NULL);
            if (pathChars) {
                result = SDL_strdup(pathChars);
                (*env)->ReleaseStringUTFChars(env, jPath, pathChars);
            }
            (*env)->DeleteLocalRef(env, jPath);
        }
    }

    (*env)->DeleteLocalRef(env, activity);
    (*env)->DeleteLocalRef(env, clazz);
    return result;
}

int Platform_Android_IsPickerActive(void) {
    JNIEnv* env = (JNIEnv*)SDL_GetAndroidJNIEnv();
    jobject activity = (jobject)SDL_GetAndroidActivity();
    jclass clazz = (*env)->GetObjectClass(env, activity);
    jmethodID methodId = (*env)->GetStaticMethodID(env, clazz, "isPickerActive", "()Z");

    jboolean result = JNI_FALSE;
    if (methodId) {
        result = (*env)->CallStaticBooleanMethod(env, clazz, methodId);
    }

    (*env)->DeleteLocalRef(env, activity);
    (*env)->DeleteLocalRef(env, clazz);
    return result == JNI_TRUE;
}

#endif
