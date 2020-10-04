/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.ethicalml.kompute

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.ethicalml.kompute.databinding.ActivityKomputeJniBinding

class KomputeJni : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val binding = ActivityKomputeJniBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.komputeGifView.loadUrl("file:///android_asset/komputer-2.gif")

        binding.komputeGifView.getSettings().setUseWideViewPort(true)
        binding.komputeGifView.getSettings().setLoadWithOverviewMode(true)

        val successVulkanInit = initVulkan()
        Log.i("KomputeJni", "Vulkan Result: " + successVulkanInit)

        binding.komputeJniTextview.text = stringFromJNI()
    }

    external fun initVulkan(): Boolean

    external fun stringFromJNI(): String

    companion object {
        init {
            System.loadLibrary("kompute-jni")
        }
    }
}

