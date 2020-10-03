// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//#undef DEBUG
//#define RELEASE 1

#include <android/log.h>
//#include <android_native_app_glue.h>
//#include <cassert>
//#include <memory>
//#include <vector>
//#include <unistd.h>

#include <string.h>
#include <jni.h>

//#include "kompute/Kompute.hpp"


// Functions interacting with Android native activity
void android_main(struct android_app* state);
void terminate(void);
void handle_cmd(android_app* app, int32_t cmd);


// Android log function wrappers
static const char* kTAG = "Vulkan-Tutorial01";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))


JNIEXPORT jstring JNICALL
Java_com_ethicalml_kompute_examples_android_KomputeJni_stringFromJNI(JNIEnv *env, jobject thiz) {
#if defined(__arm__)
    #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
#define ABI "x86"
#elif defined(__x86_64__)
#define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define ABI "mips64"
#elif defined(__mips__)
#define ABI "mips"
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#else
#define ABI "unknown"
#endif


    LOGI("Initialising vulkan");
//    if (!InitVulkan()) {
//        LOGE("Vulkan is unavailable, install vulkan and re-start");
//        return false;
//    }
//
//    LOGI("Creating manager");
//
//    kp::Manager mgr;
//
//    auto tensorA = mgr.buildTensor({0,1,2});
//    auto tensorB = mgr.buildTensor({0,1,2});
//    auto tensorC = mgr.buildTensor({1,2,3});
//
//    LOGI("Result before:");
//    for(const float & i : tensorC->data()) {
//        LOGI("%f ", i);
//    }
//
//    mgr.evalOpDefault<kp::OpMult<>>({tensorA, tensorB, tensorC});
//    mgr.evalOpDefault<kp::OpTensorSyncLocal>({tensorC});
//
//    LOGI("Result after:");
//    for(const float & i : tensorC->data()) {
//        LOGI("%f ", i);
//    }

    return (*env).NewStringUTF("Hello from JNI !  Compiled with ABI ");
}

