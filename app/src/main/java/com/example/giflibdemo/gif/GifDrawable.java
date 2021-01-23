package com.example.giflibdemo.gif;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.Drawable;
import android.os.SystemClock;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class GifDrawable extends Drawable implements Animatable, Runnable {

    private Paint mPaint;

    /**
     * 1. Java准备一个Bitmap传给C++
     * 2. C++ 解压gif每一帧（Screen）填充到Bitmap
     * 3. 把Bitmap绘制到Canvas
     */
    private Bitmap mBitmap;

    private Rect srcRect;

    private GifFrame mGifFrame;

    private int width;
    private int height;

    private boolean isRunning;

    private long showTime;
    private long curTime; // 当前帧要显示的时间

    private int frameCount;
    private int frameIndex;

    public GifDrawable(GifFrame gifFrame) {
        mGifFrame = gifFrame;

        width = mGifFrame.getWidth();
        height = mGifFrame.getHeight();
        frameCount = mGifFrame.getFrameCount();
        mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);

        mPaint = new Paint();
        mPaint.setFilterBitmap(true);
        frameIndex = 0;
        srcRect = new Rect(0, 0, width, height);
        mGifFrame.getFrame(mBitmap, getFrameIndex());
    }

    public int getFrameIndex() {
        frameIndex++;
        return frameIndex < frameCount ? frameIndex : (frameIndex = 0);  // 赋值语句的返回值是等号右边的值
    }


    @Override
    public void start() {
        if (!isRunning()) {
            isRunning = true;
            scheduleSelf(this, SystemClock.uptimeMillis());
        }
    }

    @Override
    public void stop() {
        if (isRunning()) {
            isRunning = false;
            unscheduleSelf(this);
        }
    }

    @Override
    public boolean isRunning() {
        return isRunning;
    }

    @Override
    public void draw(@NonNull Canvas canvas) {
        canvas.drawBitmap(mBitmap, srcRect, getBounds(), mPaint);
        showTime = SystemClock.uptimeMillis();

        if (isRunning()) {
            scheduleSelf(this, showTime + curTime);  // important
        }
    }

    @Override
    public void setAlpha(int i) {
        mPaint.setAlpha(i);
    }

    @Override
    public void setColorFilter(@Nullable ColorFilter colorFilter) {
        mPaint.setColorFilter(colorFilter);
    }

    @Override
    public int getOpacity() {
        return PixelFormat.TRANSLUCENT;
    }

    @Override
    public void run() {
        curTime = mGifFrame.getFrame(mBitmap, getFrameIndex());
        invalidateSelf();
    }
}
