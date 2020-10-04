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

#undef DEBUG
#define RELEASE 1

#include <android/log.h>
//#include <android_native_app_glue.h>
//#include <cassert>
//#include <memory>
//#include <vector>
#include <unistd.h>

#include <string.h>
#include <jni.h>

#include "kompute/Kompute.hpp"

#ifndef KOMPUTE_VK_INIT_RETRIES
#define KOMPUTE_VK_INIT_RETRIES 5
#endif

// Android log function wrappers
static const char* kTAG = "KomputeJni";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

static std::vector<float> jfloatArrayToVector(JNIEnv *env, const jfloatArray & fromArray) {
    float *inCArray = env->GetFloatArrayElements(fromArray, NULL);
    if (NULL == inCArray) return std::vector<float>();
    int32_t length = env->GetArrayLength(fromArray);

    std::vector<float> outVector(inCArray, inCArray + length);
    return outVector;
}

static jfloatArray vectorToJFloatArray(JNIEnv *env, const std::vector<float> & fromVector) {
    jfloatArray ret = env->NewFloatArray(fromVector.size());
    if (NULL == ret) return NULL;
    env->SetFloatArrayRegion(ret, 0, fromVector.size(), fromVector.data());
    return ret;
}

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_ethicalml_kompute_KomputeJni_initVulkan(JNIEnv *env, jobject thiz) {

    LOGI("Initialising vulkan");

    uint32_t totalRetries = 0;

    while (totalRetries < KOMPUTE_VK_INIT_RETRIES) {
        LOGI("VULKAN LOAD TRY NUMBER: %u", totalRetries);
        if(InitVulkan()) {
            break;
        }
        sleep(1);
        totalRetries++;
    }

    return totalRetries < KOMPUTE_VK_INIT_RETRIES;
}


JNIEXPORT jfloatArray JNICALL
Java_com_ethicalml_kompute_KomputeJni_kompute(
        JNIEnv *env,
        jobject thiz,
        jfloatArray xiJFloatArr,
        jfloatArray xjJFloatArr,
        jfloatArray yJFloatArr) {

    LOGI("Creating manager");

    std::vector<float> xiVector = jfloatArrayToVector(env, xiJFloatArr);
    std::vector<float> xjVector = jfloatArrayToVector(env, xjJFloatArr);
    std::vector<float> yVector = jfloatArrayToVector(env, yJFloatArr);

    kp::Manager mgr;

    auto tensorA = mgr.buildTensor(xiVector);
    auto tensorB = mgr.buildTensor(xjVector);
    auto tensorC = mgr.buildTensor(yVector);

    LOGI("Result before:");
    for(const float & i : tensorC->data()) {
        LOGI("%f ", i);
    }

    mgr.evalOpDefault<kp::OpMult<>>({tensorA, tensorB, tensorC});
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({tensorC});

    LOGI("Result after:");
    for(const float & i : tensorC->data()) {
        LOGI("%f ", i);
    }

    return vectorToJFloatArray(env, tensorC->data());
}
}
