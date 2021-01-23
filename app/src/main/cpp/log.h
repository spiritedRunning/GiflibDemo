//
// Created by Zach on 2021/1/19.
//

#ifndef GIFLIBDEMO_LOG_H
#define GIFLIBDEMO_LOG_H

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "GiflibDemo", __VA_ARGS__)


#endif //GIFLIBDEMO_LOG_H
