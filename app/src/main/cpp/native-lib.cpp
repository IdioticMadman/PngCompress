#include <jni.h>
#include <vector>
#include <iostream>
#include "libimagequant.h"
#include "log.hpp"
#include "png.h"
#define NATIVE_TAG "png-compress"

struct FreeDeleter {
    void operator()(void *ptr) const {
        free(ptr);
    }
};

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_rober_pngcompress_PngQuantize_nativeQuantizeImage(JNIEnv *env, jobject thiz,
                                                           jbyteArray inputPixels, jint width,
                                                           jint height, jint min_quality,
                                                           jint max_quality, jint max_color,
                                                           jint speed, jint min_opacity) {
    LOGI(NATIVE_TAG, "start quantize image");
    jsize length = env->GetArrayLength(inputPixels);
    if (length <= 0) {
        return nullptr;
    }
    jbyte *elements = env->GetByteArrayElements(inputPixels, JNI_FALSE);
    std::unique_ptr<signed char, FreeDeleter> pixels(
            static_cast<signed char *>(malloc(length)));
    if (!pixels) {
        LOGE(NATIVE_TAG, "malloc pixels failed");
        return nullptr;
    }
    memcpy(pixels.get(), elements, length);
    env->ReleaseByteArrayElements(inputPixels, elements, JNI_ABORT);
    std::unique_ptr<liq_attr, decltype(&liq_attr_destroy)> handle(liq_attr_create(),
                                                                  liq_attr_destroy);
    if (min_opacity != -1) {
        liq_set_min_opacity(handle.get(), min_opacity);
    }
    if (min_quality != -1 && max_quality != -1) {
        liq_set_quality(handle.get(), min_quality, max_quality);
    }
    if (max_color != -1) {
        liq_set_max_colors(handle.get(), max_color);
    }
    if (speed != -1) {
        liq_set_speed(handle.get(), speed);
    }
    std::unique_ptr<liq_image, decltype(&liq_image_destroy)> input_image(
            liq_image_create_rgba(handle.get(), pixels.get(), width, height, 0),
            liq_image_destroy);

    liq_result *raw_quantization_result = nullptr;
    if (liq_image_quantize(input_image.get(), handle.get(), &raw_quantization_result) != LIQ_OK) {
        LOGE(NATIVE_TAG, "Quantization failed");
        return nullptr;
    }
    std::unique_ptr<liq_result, decltype(&liq_result_destroy)> quantization_result(
            raw_quantization_result, liq_result_destroy);

    size_t pixels_size = width * height;
    std::unique_ptr<unsigned char, FreeDeleter> raw_8bit_pixels(
            static_cast<unsigned char *>(malloc(pixels_size)));

    if (!raw_8bit_pixels) {
        LOGE(NATIVE_TAG, "Malloc failed for raw_8bit_pixels");
        return nullptr;
    }

    liq_set_dithering_level(quantization_result.get(), 1.0);
    liq_write_remapped_image(quantization_result.get(), input_image.get(), raw_8bit_pixels.get(),
                             pixels_size);
    const liq_palette *palette = liq_get_palette(quantization_result.get());

    // 准备libpng写入过程
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        LOGE(NATIVE_TAG, "Unable to create PNG write struct");
        return nullptr;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
        LOGE(NATIVE_TAG, "Unable to create PNG info struct");
        return nullptr;
    }

    // 设置错误处理
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        LOGE(NATIVE_TAG, "Error during PNG creation");
        return nullptr;
    }

    // 准备内存输出
    std::vector<unsigned char> png_data;
    png_set_write_fn(png_ptr, &png_data, [](png_structp png_ptr, png_bytep data, png_size_t length) {
        auto p = static_cast<std::vector<unsigned char>*>(png_get_io_ptr(png_ptr));
        p->insert(p->end(), data, data + length);
    }, nullptr);

    // 设置PNG头信息
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_PALETTE,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // 设置调色板
    png_colorp png_palette = (png_colorp)png_malloc(png_ptr, palette->count * sizeof(png_color));

    png_bytep trans_alpha = nullptr; // 用于存储alpha值的数组
    int num_trans = 0; // 透明颜色的数量
    trans_alpha = (png_bytep)png_malloc(png_ptr, palette->count * sizeof(png_byte));

    for (int i = 0; i < palette->count; i++) {
        png_palette[i].red = palette->entries[i].r;
        png_palette[i].green = palette->entries[i].g;
        png_palette[i].blue = palette->entries[i].b;
        trans_alpha[i] = palette->entries[i].a;
        if (trans_alpha[i] < 255) {
            num_trans++; // 如果alpha值小于255，增加透明颜色的数量
        }
    }
    png_set_PLTE(png_ptr, info_ptr, png_palette, palette->count);
    if (num_trans > 0) {
        // 如果有透明颜色，设置透明度信息
        png_set_tRNS(png_ptr, info_ptr, trans_alpha, num_trans, nullptr);
    }
    png_free(png_ptr, png_palette);
    png_free(png_ptr, trans_alpha);
    // 写入头信息
    png_write_info(png_ptr, info_ptr);

    // 写入图像数据
    for (int y = 0; y < height; y++) {
        png_write_row(png_ptr, &raw_8bit_pixels.get()[y * width]);
    }

    // 完成写入
    png_write_end(png_ptr, nullptr);

    // 清理
    png_destroy_write_struct(&png_ptr, &info_ptr);

    // 将PNG数据转换为Java字节数组
    jbyteArray pngFileData = env->NewByteArray(png_data.size());
    if (pngFileData != nullptr) {
        env->SetByteArrayRegion(pngFileData, 0, png_data.size(),
                                reinterpret_cast<const jbyte *>(png_data.data()));
        LOGI(NATIVE_TAG, "new png file data success");
        return pngFileData;
    }
    return nullptr;
}