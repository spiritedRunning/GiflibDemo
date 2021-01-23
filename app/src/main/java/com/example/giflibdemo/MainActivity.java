package com.example.giflibdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.giflibdemo.gif.GifDrawable;
import com.example.giflibdemo.gif.GifFrame;

import java.io.FileInputStream;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE}, 666);
        }

//            File file = new File("a.txt");
//            FileInputStream fileInputStream = new FileInputStream(file);
//            byte[] buffer = new byte[1024];
//            fileInputStream.read(buffer,0,buffer.length);

        ImageView imageView = findViewById(R.id.image);
        try {
            // TODO 1.gif 2.gif 播放异常， 待研究
//            GifDrawable gifDrawable = new GifDrawable(GifFrame.decodeStream(getAssets().open("6.gif")));

//            GifDrawable gifDrawable = new GifDrawable(GifFrame.decodeStream(null, "/sdcard/6.gif"));
            GifDrawable gifDrawable = new GifDrawable(GifFrame.decodeStream(this, "6.gif"));

            imageView.setImageDrawable(gifDrawable);
            gifDrawable.start();
        } catch (Exception e) {
            e.printStackTrace();
        }


    }
}
