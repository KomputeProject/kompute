#pragma once
/// \file key.hpp Key-mappping between C++ and GLFW
#include <glfw/config.hpp>

namespace glfw {

enum class key : int {
  UNKNOWN = GLFW_KEY_UNKNOWN,              //
  SPACE = GLFW_KEY_SPACE,                  //
  APOSTROPHE = GLFW_KEY_APOSTROPHE,        //
  COMMA = GLFW_KEY_COMMA,                  //
  MINUS = GLFW_KEY_MINUS,                  //
  PERIOD = GLFW_KEY_PERIOD,                //
  SLASH = GLFW_KEY_SLASH,                  //
  K0 = GLFW_KEY_0,                         //
  K1 = GLFW_KEY_1,                         //
  K2 = GLFW_KEY_2,                         //
  K3 = GLFW_KEY_3,                         //
  K4 = GLFW_KEY_4,                         //
  K5 = GLFW_KEY_5,                         //
  K6 = GLFW_KEY_6,                         //
  K7 = GLFW_KEY_7,                         //
  K8 = GLFW_KEY_8,                         //
  K9 = GLFW_KEY_9,                         //
  SEMICOLON = GLFW_KEY_SEMICOLON,          //
  EQUAL = GLFW_KEY_EQUAL,                  //
  A = GLFW_KEY_A,                          //
  B = GLFW_KEY_B,                          //
  C = GLFW_KEY_C,                          //
  D = GLFW_KEY_D,                          //
  E = GLFW_KEY_E,                          //
  F = GLFW_KEY_F,                          //
  G = GLFW_KEY_G,                          //
  H = GLFW_KEY_H,                          //
  I = GLFW_KEY_I,                          //
  J = GLFW_KEY_J,                          //
  K = GLFW_KEY_K,                          //
  L = GLFW_KEY_L,                          //
  M = GLFW_KEY_M,                          //
  N = GLFW_KEY_N,                          //
  O = GLFW_KEY_O,                          //
  P = GLFW_KEY_P,                          //
  Q = GLFW_KEY_Q,                          //
  R = GLFW_KEY_R,                          //
  S = GLFW_KEY_S,                          //
  T = GLFW_KEY_T,                          //
  U = GLFW_KEY_U,                          //
  V = GLFW_KEY_V,                          //
  W = GLFW_KEY_W,                          //
  X = GLFW_KEY_X,                          //
  Y = GLFW_KEY_Y,                          //
  Z = GLFW_KEY_Z,                          //
  LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,    //
  BACKSLASH = GLFW_KEY_BACKSLASH,          //
  RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,  //
  GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,    //
  WORLD_1 = GLFW_KEY_WORLD_1,              //
  WORLD_2 = GLFW_KEY_WORLD_2,              //
  ESCAPE = GLFW_KEY_ESCAPE,                //
  ENTER = GLFW_KEY_ENTER,                  //
  TAB = GLFW_KEY_TAB,                      //
  BACKSPACE = GLFW_KEY_BACKSPACE,          //
  INSERT = GLFW_KEY_INSERT,                //
  RIGHT = GLFW_KEY_RIGHT,                  //
  LEFT = GLFW_KEY_LEFT,                    //
  DOWN = GLFW_KEY_DOWN,                    //
  UP = GLFW_KEY_UP,                        //
  PAGE_UP = GLFW_KEY_PAGE_UP,              //
  PAGE_DOWN = GLFW_KEY_PAGE_DOWN,          //
  HOME = GLFW_KEY_HOME,                    //
  END = GLFW_KEY_END,                      //
  CAPS_LOCK = GLFW_KEY_CAPS_LOCK,          //
  SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,      //
  NUM_LOCK = GLFW_KEY_NUM_LOCK,            //
  PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,    //
  PAUSE = GLFW_KEY_PAUSE,                  //
  F1 = GLFW_KEY_F1,                        //
  F2 = GLFW_KEY_F2,                        //
  F3 = GLFW_KEY_F3,                        //
  F4 = GLFW_KEY_F4,                        //
  F5 = GLFW_KEY_F5,                        //
  F6 = GLFW_KEY_F6,                        //
  F7 = GLFW_KEY_F7,                        //
  F8 = GLFW_KEY_F8,                        //
  F9 = GLFW_KEY_F9,                        //
  F10 = GLFW_KEY_F10,                      //
  F11 = GLFW_KEY_F11,                      //
  F12 = GLFW_KEY_F12,                      //
  F13 = GLFW_KEY_F13,                      //
  F14 = GLFW_KEY_F14,                      //
  F15 = GLFW_KEY_F15,                      //
  F16 = GLFW_KEY_F16,                      //
  F17 = GLFW_KEY_F17,                      //
  F18 = GLFW_KEY_F18,                      //
  F19 = GLFW_KEY_F19,                      //
  F20 = GLFW_KEY_F20,                      //
  F21 = GLFW_KEY_F21,                      //
  F22 = GLFW_KEY_F22,                      //
  F23 = GLFW_KEY_F23,                      //
  F24 = GLFW_KEY_F24,                      //
  F25 = GLFW_KEY_F25,                      //
  KP_0 = GLFW_KEY_KP_0,                    //
  KP_1 = GLFW_KEY_KP_1,                    //
  KP_2 = GLFW_KEY_KP_2,                    //
  KP_3 = GLFW_KEY_KP_3,                    //
  KP_4 = GLFW_KEY_KP_4,                    //
  KP_5 = GLFW_KEY_KP_5,                    //
  KP_6 = GLFW_KEY_KP_6,                    //
  KP_7 = GLFW_KEY_KP_7,                    //
  KP_8 = GLFW_KEY_KP_8,                    //
  KP_9 = GLFW_KEY_KP_9,                    //
  KP_DECIMAL = GLFW_KEY_KP_DECIMAL,        //
  KP_DIVIDE = GLFW_KEY_KP_DIVIDE,          //
  KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,      //
  KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,      //
  KP_ADD = GLFW_KEY_KP_ADD,                //
  KP_ENTER = GLFW_KEY_KP_ENTER,            //
  KP_EQUAL = GLFW_KEY_KP_EQUAL,            //
  LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,        //
  LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,    //
  LEFT_ALT = GLFW_KEY_LEFT_ALT,            //
  LEFT_SUPER = GLFW_KEY_LEFT_SUPER,        //
  RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,      //
  RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,  //
  RIGHT_ALT = GLFW_KEY_RIGHT_ALT,          //
  RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,      //
  MENU = GLFW_KEY_MENU
};

std::string to_string(const key v) {
  switch (v) {
    case key::UNKNOWN: {
      return "key::UNKNOWN";
    }
    case key::SPACE: {
      return "key::SPACE";
    }
    case key::APOSTROPHE: {
      return "key::APOSTROPHE";
    }
    case key::COMMA: {
      return "key::COMMA";
    }
    case key::MINUS: {
      return "key::MINUS";
    }
    case key::PERIOD: {
      return "key::PERIOD";
    }
    case key::SLASH: {
      return "key::SLASH";
    }
    case key::K0: {
      return "key::K0";
    }
    case key::K1: {
      return "key::K1";
    }
    case key::K2: {
      return "key::K2";
    }
    case key::K3: {
      return "key::K3";
    }
    case key::K4: {
      return "key::K4";
    }
    case key::K5: {
      return "key::K5";
    }
    case key::K6: {
      return "key::K6";
    }
    case key::K7: {
      return "key::K7";
    }
    case key::K8: {
      return "key::K8";
    }
    case key::K9: {
      return "key::K9";
    }
    case key::SEMICOLON: {
      return "key::SEMICOLON";
    }
    case key::EQUAL: {
      return "key::EQUAL";
    }
    case key::A: {
      return "key::A";
    }
    case key::B: {
      return "key::B";
    }
    case key::C: {
      return "key::C";
    }
    case key::D: {
      return "key::D";
    }
    case key::E: {
      return "key::E";
    }
    case key::F: {
      return "key::F";
    }
    case key::G: {
      return "key::G";
    }
    case key::H: {
      return "key::H";
    }
    case key::I: {
      return "key::I";
    }
    case key::J: {
      return "key::J";
    }
    case key::K: {
      return "key::K";
    }
    case key::L: {
      return "key::L";
    }
    case key::M: {
      return "key::M";
    }
    case key::N: {
      return "key::N";
    }
    case key::O: {
      return "key::O";
    }
    case key::P: {
      return "key::P";
    }
    case key::Q: {
      return "key::Q";
    }
    case key::R: {
      return "key::R";
    }
    case key::S: {
      return "key::S";
    }
    case key::T: {
      return "key::T";
    }
    case key::U: {
      return "key::U";
    }
    case key::V: {
      return "key::V";
    }
    case key::W: {
      return "key::W";
    }
    case key::X: {
      return "key::X";
    }
    case key::Y: {
      return "key::Y";
    }
    case key::Z: {
      return "key::Z";
    }
    case key::LEFT_BRACKET: {
      return "key::LEFT_BRACKET";
    }
    case key::BACKSLASH: {
      return "key::BACKSLASH";
    }
    case key::RIGHT_BRACKET: {
      return "key::RIGHT_BRACKET";
    }
    case key::GRAVE_ACCENT: {
      return "key::GRAVE_ACCENT";
    }
    case key::WORLD_1: {
      return "key::WORLD_1";
    }
    case key::WORLD_2: {
      return "key::WORLD_2";
    }
    case key::ESCAPE: {
      return "key::ESCAPE";
    }
    case key::ENTER: {
      return "key::ENTER";
    }
    case key::TAB: {
      return "key::TAB";
    }
    case key::BACKSPACE: {
      return "key::BACKSPACE";
    }
    case key::INSERT: {
      return "key::INSERT";
    }
    case key::RIGHT: {
      return "key::RIGHT";
    }
    case key::LEFT: {
      return "key::LEFT";
    }
    case key::DOWN: {
      return "key::DOWN";
    }
    case key::UP: {
      return "key::UP";
    }
    case key::PAGE_UP: {
      return "key::PAGE_UP";
    }
    case key::PAGE_DOWN: {
      return "key::PAGE_DOWN";
    }
    case key::HOME: {
      return "key::HOME";
    }
    case key::END: {
      return "key::END";
    }
    case key::CAPS_LOCK: {
      return "key::CAPS_LOCK";
    }
    case key::SCROLL_LOCK: {
      return "key::SCROLL_LOCK";
    }
    case key::NUM_LOCK: {
      return "key::NUM_LOCK";
    }
    case key::PRINT_SCREEN: {
      return "key::PRINT_SCREEN";
    }
    case key::PAUSE: {
      return "key::PAUSE";
    }
    case key::F1: {
      return "key::F1";
    }
    case key::F2: {
      return "key::F2";
    }
    case key::F3: {
      return "key::F3";
    }
    case key::F4: {
      return "key::F4";
    }
    case key::F5: {
      return "key::F5";
    }
    case key::F6: {
      return "key::F6";
    }
    case key::F7: {
      return "key::F7";
    }
    case key::F8: {
      return "key::F8";
    }
    case key::F9: {
      return "key::F9";
    }
    case key::F10: {
      return "key::F10";
    }
    case key::F11: {
      return "key::F11";
    }
    case key::F12: {
      return "key::F12";
    }
    case key::F13: {
      return "key::F13";
    }
    case key::F14: {
      return "key::F14";
    }
    case key::F15: {
      return "key::F15";
    }
    case key::F16: {
      return "key::F16";
    }
    case key::F17: {
      return "key::F17";
    }
    case key::F18: {
      return "key::F18";
    }
    case key::F19: {
      return "key::F19";
    }
    case key::F20: {
      return "key::F20";
    }
    case key::F21: {
      return "key::F21";
    }
    case key::F22: {
      return "key::F22";
    }
    case key::F23: {
      return "key::F23";
    }
    case key::F24: {
      return "key::F24";
    }
    case key::F25: {
      return "key::F25";
    }
    case key::KP_0: {
      return "key::KP_0";
    }
    case key::KP_1: {
      return "key::KP_1";
    }
    case key::KP_2: {
      return "key::KP_2";
    }
    case key::KP_3: {
      return "key::KP_3";
    }
    case key::KP_4: {
      return "key::KP_4";
    }
    case key::KP_5: {
      return "key::KP_5";
    }
    case key::KP_6: {
      return "key::KP_6";
    }
    case key::KP_7: {
      return "key::KP_7";
    }
    case key::KP_8: {
      return "key::KP_8";
    }
    case key::KP_9: {
      return "key::KP_9";
    }
    case key::KP_DECIMAL: {
      return "key::KP_DECIMAL";
    }
    case key::KP_DIVIDE: {
      return "key::KP_DIVIDE";
    }
    case key::KP_MULTIPLY: {
      return "key::KP_MULTIPLY";
    }
    case key::KP_SUBTRACT: {
      return "key::KP_SUBTRACT";
    }
    case key::KP_ADD: {
      return "key::KP_ADD";
    }
    case key::KP_ENTER: {
      return "key::KP_ENTER";
    }
    case key::KP_EQUAL: {
      return "key::KP_EQUAL";
    }
    case key::LEFT_SHIFT: {
      return "key::LEFT_SHIFT";
    }
    case key::LEFT_CONTROL: {
      return "key::LEFT_CONTROL";
    }
    case key::LEFT_ALT: {
      return "key::LEFT_ALT";
    }
    case key::LEFT_SUPER: {
      return "key::LEFT_SUPER";
    }
    case key::RIGHT_SHIFT: {
      return "key::RIGHT_SHIFT";
    }
    case key::RIGHT_CONTROL: {
      return "key::RIGHT_CONTROL";
    }
    case key::RIGHT_ALT: {
      return "key::RIGHT_ALT";
    }
    case key::RIGHT_SUPER: {
      return "key::RIGHT_SUPER";
    }
    case key::MENU: {
      return "key::MENU";
    }
    default: { report_logic_error("unkown key"); }
  }
}

}  // namespace glfw
