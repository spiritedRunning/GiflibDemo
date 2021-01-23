//
// Created by Zach on 2021/1/19.
//


#include "GifFrame.h"
#include "log.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>

#define ARGB_TO_COLOR8888(a, r, g, b) \
((a) << 24 | (b) << 16 | (g) << 8 | (r))


static int readStream(GifFileType* fileType, GifByteType *out, int size) {
    JavaInputStream* stream = static_cast<JavaInputStream *>(fileType->UserData);
    return (int)stream->read(out, size);
}

static int readStreamFromAsset(GifFileType* fileType, GifByteType *out, int size) {
    AAsset* asset = static_cast<AAsset *>(fileType->UserData);
    return AAsset_read(asset, out, size);

}


static long getDelayMs(GraphicsControlBlock &gcb) {
    return gcb.DelayTime * 10;
}


GifFrame::GifFrame(JavaInputStream *stream) {
    int error;
    mGif = DGifOpen(stream, readStream, &error);
    if (mGif == nullptr) {
        LOGI("DGifOpen error: %d", error);
        return;
    }
    // 初始化结构图
    DGifSlurp(mGif);

    GraphicsControlBlock gcb;
    long durationMs = 0;
    for (int i = 0; i < mGif->ImageCount; i++) {
        // 获取显示时间
        DGifSavedExtensionToGCB(mGif, i, &gcb);
        durationMs += getDelayMs(gcb);
    }
}

GifFrame::GifFrame(JNIEnv *env, jobject assetManager, const char *filename) {
    LOGI("filename: %s", filename);

    if (assetManager) {
        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
        AAsset *asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
        mGif = DGifOpen(asset, readStreamFromAsset, NULL);
    } else {
        mGif = DGifOpenFileName(filename, NULL);
    }

    if (mGif) {
        DGifSlurp(mGif);
        GraphicsControlBlock gcb;
        long durationMs = 0;
        for (int i = 0; i < mGif->ImageCount; i++) {
            DGifSavedExtensionToGCB(mGif, i, &gcb);
            durationMs += getDelayMs(gcb);
        }
    }
}

GifFrame::~GifFrame() {

}

int GifFrame::getWidth() {
    return mGif ? mGif->SWidth : 0;
}

int GifFrame::getHeight() {
    return mGif ? mGif->SHeight : 0;
}

int GifFrame::getFrameCount() {
    return mGif ? mGif->ImageCount : 0;
}

/**
 * 把屏幕（gif逻辑上的屏幕）上的内容填充到bitmap
 */
long GifFrame::loadFrame(JNIEnv *env, jobject bitmap, int frameIndex) {
    AndroidBitmapInfo info;
    uint32_t* pixels;

    AndroidBitmap_getInfo(env, bitmap, &info);
    AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixels));

    // 对像素缓冲区进行相应的修改，使其符合相应的像素格式、宽度和其他特性
    SavedImage savedImage = mGif->SavedImages[frameIndex];
    GifImageDesc imageDesc = savedImage.ImageDesc;
    ColorMapObject* colorMapObject = mGif->SColorMap ? mGif->SColorMap : imageDesc.ColorMap;

    for (int i = 0; i < imageDesc.Height; i++) {
        for (int j = 0; j < imageDesc.Width; j++) { // 把每个像素点
            int index = i * imageDesc.Width + j;

            GifByteType  gifByteType = savedImage.RasterBits[index]; // 像素点是压缩的
            GifColorType gifColorType = colorMapObject->Colors[gifByteType];
            uint32_t color = ARGB_TO_COLOR8888(0xff, gifColorType.Red, gifColorType.Green, gifColorType.Blue);
            pixels[index] = color;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    GraphicsControlBlock gcb;
    DGifSavedExtensionToGCB(mGif, frameIndex, &gcb);
    return getDelayMs(gcb);
}
