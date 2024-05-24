package com.rober.pngcompress

import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.rober.pngcompress.databinding.ActivityMainBinding
import java.io.File
import java.io.FileOutputStream
import java.nio.ByteBuffer
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        // Example of a call to a native method
        thread {
            val bitmap = assets.open("test.png").use { BitmapFactory.decodeStream(it) }
            val byteBuffer = ByteBuffer.allocateDirect(bitmap.byteCount)
            bitmap.copyPixelsToBuffer(byteBuffer)
            val start = System.currentTimeMillis()
            val quantizeImage = PngQuantize.nativeQuantizeImage(
                byteBuffer.array(), bitmap.width, bitmap.height, -1, -1, -1, -1, -1
            )
            val cost = System.currentTimeMillis() - start
            Log.i(TAG, "cost: $cost")
            if (quantizeImage != null) {
                val file = File(externalCacheDir, "output-2.png")
                if (file.exists()) {
                    file.delete()
                }
                FileOutputStream(file).use {
                    it.write(quantizeImage)
                    it.flush()
                }
                Log.i(TAG, "file path :${file.absolutePath}")
            } else {
                Log.i(TAG, "quantizeImage image failed")
            }
        }
    }


    companion object {
        private const val TAG = "MainActivity"
    }
}