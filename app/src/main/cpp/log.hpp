//
// Created by roberxiong on 2024/5/15.
//

#ifndef CPPDEM1_LOG_HPP
#define CPPDEM1_LOG_HPP

#if defined(__ANDROID__)
#include <android/log.h>
#define LOGI(TAG, ...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOGD(TAG, ...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOGW(TAG, ...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))
#define LOGE(TAG, ...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#elif defined(__APPLE__)
#include <Foundation/Foundation.h>
#define LOGI(...) NSLog(@"[INFO] %s", [[NSString stringWithFormat:__VA_ARGS__] UTF8String])
#define LOGD(...) NSLog(@"[DEBUG] %s", [[NSString stringWithFormat:__VA_ARGS__] UTF8String])
#define LOGW(...) NSLog(@"[WARN] %s", [[NSString stringWithFormat:__VA_ARGS__] UTF8String])
#define LOGE(...) NSLog(@"[ERROR] %s", [[NSString stringWithFormat:__VA_ARGS__] UTF8String])
#else
#define LOGI(...) printf("[INFO] " __VA_ARGS__)
#define LOGD(...) printf("[DEBUG] " __VA_ARGS__)
#define LOGW(...) printf("[WARN] " __VA_ARGS__)
#define LOGE(...) printf("[ERROR] " __VA_ARGS__)
#endif

#endif //CPPDEM1_LOG_HPP
