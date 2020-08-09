/*
* Key codes for multiple platforms
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#if defined(__ANDROID__)

#include <android/keycodes.h>

#define KEY_ESCAPE AKEYCODE_ESCAPE
#define KEY_F1 AKEYCODE_F1
#define KEY_F2 AKEYCODE_F2
#define KEY_F3 AKEYCODE_F3
#define KEY_F4 AKEYCODE_F4
#define KEY_F5 AKEYCODE_F5
#define KEY_W AKEYCODE_W
#define KEY_A AKEYCODE_A
#define KEY_S AKEYCODE_S
#define KEY_D AKEYCODE_D
#define KEY_G AKEYCODE_G
#define KEY_P AKEYCODE_P
#define KEY_SPACE AKEYCODE_SPACE
#define KEY_KPADD AKEYCODE_NUMPAD_ADD
#define KEY_KPSUB AKEYCODE_NUMPAD_SUBTRACT
#define KEY_B AKEYCODE_B
#define KEY_F AKEYCODE_F
#define KEY_L AKEYCODE_L
#define KEY_N AKEYCODE_N
#define KEY_O AKEYCODE_O
#define KEY_T AKEYCODE_T

#define GAMEPAD_BUTTON_A 0x1000
#define GAMEPAD_BUTTON_B 0x1001
#define GAMEPAD_BUTTON_X 0x1002
#define GAMEPAD_BUTTON_Y 0x1003
#define GAMEPAD_BUTTON_L1 0x1004
#define GAMEPAD_BUTTON_R1 0x1005
#define GAMEPAD_BUTTON_START 0x1006
#define TOUCH_DOUBLE_TAP 0x1100

#else

#define KEY_ESCAPE GLFW_KEY_ESCAPE
#define KEY_F1 GLFW_KEY_F1
#define KEY_F2 GLFW_KEY_F2
#define KEY_F3 GLFW_KEY_F3
#define KEY_F4 GLFW_KEY_F4
#define KEY_F5 GLFW_KEY_F5
#define KEY_A GLFW_KEY_A
#define KEY_B GLFW_KEY_B
#define KEY_D GLFW_KEY_D
#define KEY_F GLFW_KEY_F
#define KEY_G GLFW_KEY_G
#define KEY_L GLFW_KEY_L
#define KEY_N GLFW_KEY_N
#define KEY_O GLFW_KEY_O
#define KEY_P GLFW_KEY_P
#define KEY_S GLFW_KEY_S
#define KEY_T GLFW_KEY_T
#define KEY_W GLFW_KEY_W
#define KEY_SPACE GLFW_KEY_SPACE
#define KEY_KPADD GLFW_KEY_KP_ADD
#define KEY_KPSUB GLFW_KEY_KP_SUBTRACT

#endif
