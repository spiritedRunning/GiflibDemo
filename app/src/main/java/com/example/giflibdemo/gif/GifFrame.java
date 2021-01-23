package com.example.giflibdemo.gif;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;

import java.io.InputStream;

public class GifFrame {

    static {
        System.loadLibrary("native-lib");
    }

    private int width;
    private int height;
    private int frameCount;

    // C++ GifFrame对象指针
    private long nativeHandle;

    private GifFrame(long nativeHandle, int width, int height, int frameCount) {
        this.nativeHandle = nativeHandle;
        this.width = width;
        this.height = height;
        this.frameCount = frameCount;
    }

    public long getFrame(Bitmap bitmap, int frameIndex) {
        return nativeGetFrame(nativeHandle, bitmap, frameIndex);
    }

    public static GifFrame decodeStream(InputStream stream) {
        byte[] buffer = new byte[16 * 1024];
        return nativeDecodeStream(stream, buffer);
    }


    public static GifFrame decodeStream(final Context context, final String filename) {
        return nativeDecodeStreamFromAsset((context == null) ? null : context.getAssets(), filename);
    }


    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getFrameCount() {
        return frameCount;
    }


    private static native GifFrame nativeDecodeStream(InputStream stream, byte[] buffer);
    private static native GifFrame nativeDecodeStreamFromAsset(AssetManager assetManager, String gifPath);

    private native long nativeGetFrame(long nativeHandle, Bitmap bitmap, int frameIndex);


}
