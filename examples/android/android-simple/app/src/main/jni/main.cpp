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
#include <android_native_app_glue.h>
#include <cassert>
#include <memory>
#include <vector>
#include <unistd.h>

#include "kompute/Kompute.hpp"


// Android log function wrappers
static const char* kTAG = "Vulkan-Tutorial01";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))


// We will call this function the window is opened.
// This is where we will initialise everything
bool initialized_ = false;
bool initialize(android_app* app);

// Functions interacting with Android native activity
void android_main(struct android_app* state);
void terminate(void);
void handle_cmd(android_app* app, int32_t cmd);

// typical Android NativeActivity entry function
void android_main(struct android_app* app) {
    app->onAppCmd = handle_cmd;

    int events;
    android_poll_source* source;
    do {
        if (ALooper_pollAll(initialized_ ? 1 : 0, nullptr, &events,
                            (void**)&source) >= 0) {
            if (source != NULL) source->process(app, source);
        }
    } while (app->destroyRequested == 0);
}

bool initialize(android_app* app) {

    LOGI("Initialising vulkan");
    if (!InitVulkan()) {
        LOGE("Vulkan is unavailable, install vulkan and re-start");
        return false;
    }

    LOGI("Creating manager");

    kp::Manager mgr;

    auto tensorA = mgr.buildTensor({0,1,2});
    auto tensorB = mgr.buildTensor({0,1,2});
    auto tensorC = mgr.buildTensor({1,2,3});

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

    initialized_ = true;

    return 0;
}

void terminate(void) {
    initialized_ = false;
}

// Process the next main command.
void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            initialize(app);
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            terminate();
            break;
        default:
            LOGI("event not handled: %d", cmd);
    }
}
