package com.rober.pngcompress

object PngQuantize {
    init {
        System.loadLibrary("PngQuantize")
    }
    external fun nativeQuantizeImage(
        pixels: ByteArray, width: Int, height: Int, minQuality: Int,
        maxQuality: Int, maxColor: Int, speed: Int, minOpacity: Int
    ): ByteArray?
}