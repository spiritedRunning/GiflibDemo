#include <jni.h>
#include <string>
#include "JavaInputStream.h"
#include "GifFrame.h"



/**
 * 把gif图片解析成GifFrame, 创建一个java的GifFrame 返回给Java层
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_giflibdemo_gif_GifFrame_nativeDecodeStream(JNIEnv *env, jclass clazz,
                                                            jobject stream, jbyteArray buffer) {
    // 1. 从Stream中创建C++层的GifFrame
    jclass inputStreamClazz = env->FindClass("java/io/InputStream");
    JavaInputStream::readMethodID = env->GetMethodID(inputStreamClazz, "read", "([BII)I");
    JavaInputStream inputStream(env, stream, buffer);
    GifFrame *gifFrame = new GifFrame(&inputStream);

    // 2. 调用Java层GifFrame构造函数
    jclass gifFrameClazz = env->FindClass("com/example/giflibdemo/gif/GifFrame");
    jmethodID gifFrameInit = env->GetMethodID(gifFrameClazz, "<init>", "(JIII)V");

    return env->NewObject(gifFrameClazz, gifFrameInit, reinterpret_cast<jlong>(gifFrame),
                          gifFrame->getWidth(), gifFrame->getHeight(), gifFrame->getFrameCount());
}



/**
 * 获取index帧的数据
 */
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_giflibdemo_gif_GifFrame_nativeGetFrame(JNIEnv *env, jobject thiz,
                                                        jlong native_handle, jobject bitmap,
                                                        jint frame_index) {
    GifFrame* gifFrame = reinterpret_cast<GifFrame *>(native_handle);
    jlong delayMs = gifFrame->loadFrame(env, bitmap, frame_index);
    return delayMs;
}



extern "C"
JNIEXPORT jobject JNICALL
decode_from_asset(JNIEnv *env, jclass clazz, jobject asset_manager, jstring gif_path) {
    // 1. 从Asset中构造C++ GifFrame
    const char* filename = env->GetStringUTFChars(gif_path, 0);
    GifFrame* gifFrame = new GifFrame(env, asset_manager, filename);
    env->ReleaseStringUTFChars(gif_path, filename);

    // 2. 调用Java层GifFrame构造函数
    jclass gifFrameClazz = env->FindClass("com/example/giflibdemo/gif/GifFrame");
    jmethodID gifFrameInit = env->GetMethodID(gifFrameClazz, "<init>", "(JIII)V");

    return env->NewObject(gifFrameClazz, gifFrameInit, reinterpret_cast<jlong>(gifFrame),
                          gifFrame->getWidth(), gifFrame->getHeight(), gifFrame->getFrameCount());
}

// 注意这里的声明
JNINativeMethod methods[] = {
        {"nativeDecodeStreamFromAsset", "(Landroid/content/res/AssetManager;Ljava/lang/String;)Lcom/example/giflibdemo/gif/GifFrame;", (void *) decode_from_asset},
};

jint registerNativeMethod(JNIEnv* env) {
    jclass cl = env->FindClass("com/example/giflibdemo/gif/GifFrame");
    if (env->RegisterNatives(cl, methods, sizeof(methods) / sizeof(methods[0])) < 0) {
        return -1;
    }

    return 0;
}

jint unRegisterNativeMethod(JNIEnv* env) {
    jclass cl = env->FindClass("com/example/giflibdemo/gif/GifFrame");
    if (env->UnregisterNatives(cl)) {
        return -1;
    }
    return 0;
}


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *param) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) == JNI_OK) {
        registerNativeMethod(env);
        return JNI_VERSION_1_6;
    } else if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) == JNI_OK) {
        registerNativeMethod(env);
        return JNI_VERSION_1_4;
    }
    return  JNI_ERR;
}


JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *param) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) == JNI_OK) {
        unRegisterNativeMethod(env);
    } else if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) == JNI_OK) {
        unRegisterNativeMethod(env);
    }
}
