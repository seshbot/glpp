#include <glpp/input.h>

#include "GLFW/glfw3.h"

namespace glpp {

   std::vector<Key> Keys::All = Keys::Range(Key::KEY_SPACE, Key::KEY_MENU);
   std::vector<Key> Keys::Numbers = Keys::Range(Key::KEY_0, Key::KEY_9);
   std::vector<Key> Keys::Range(Key first, Key last) {
      std::vector<Key> result;
      result.reserve(last - first);

      using int_type = std::underlying_type_t<Key>;
      for (auto k = static_cast<int_type>(first); k <= last; k++){
         result.push_back(static_cast<Key>(k));
      }

      return result;
   }

   std::string to_string(KeyModFlag mods) {
      
      switch (mods) {
      case KEY_MOD_SHIFT: return "KEY_MOD_SHIFT";
      case KEY_MOD_CONTROL: return "KEY_MOD_CONTROL";
      case KEY_MOD_ALT: return "KEY_MOD_ALT";
      case KEY_MOD_SUPER: return "KEY_MOD_SUPER";
      default: return "unknown";
      }
   }

   std::string key_mods_to_string(int mods) {
      std::string ret{ "scax" };
      if (mods & KEY_MOD_SHIFT) ret[0] = 'S';
      if (mods & KEY_MOD_CONTROL) ret[1] = 'C';
      if (mods & KEY_MOD_ALT) ret[2] = 'A';
      if (mods & KEY_MOD_SUPER) ret[3] = 'X';
      return ret;
   }

   int from_glfw_key_mods(int mods) {
      return mods; // they should be the same
   }

   std::string to_string(KeyAction action);
   KeyAction from_glfw_key_action(int action) {
      switch (action) {
      case GLFW_PRESS: return KEY_ACTION_PRESS;
      case GLFW_RELEASE: return KEY_ACTION_RELEASE;
      case GLFW_REPEAT: return KEY_ACTION_REPEAT;
      default: return KEY_ACTION_UNKNOWN;
      }
   }

   std::string to_string(Key key) {
      switch (key) {
      case KEY_SPACE: return "SPACE";
      case KEY_APOSTROPHE: return "APOSTROPHE";
      case KEY_COMMA: return "COMMA";
      case KEY_MINUS: return "MINUS";
      case KEY_PERIOD: return "PERIOD";
      case KEY_SLASH: return "SLASH";
      case KEY_0: return "0";
      case KEY_1: return "1";
      case KEY_2: return "2";
      case KEY_3: return "3";
      case KEY_4: return "4";
      case KEY_5: return "5";
      case KEY_6: return "6";
      case KEY_7: return "7";
      case KEY_8: return "8";
      case KEY_9: return "9";
      case KEY_SEMICOLON: return "SEMICOLON";
      case KEY_EQUAL: return "EQUAL";
      case KEY_A: return "A";
      case KEY_B: return "B";
      case KEY_C: return "C";
      case KEY_D: return "D";
      case KEY_E: return "E";
      case KEY_F: return "F";
      case KEY_G: return "G";
      case KEY_H: return "H";
      case KEY_I: return "I";
      case KEY_J: return "J";
      case KEY_K: return "K";
      case KEY_L: return "L";
      case KEY_M: return "M";
      case KEY_N: return "N";
      case KEY_O: return "O";
      case KEY_P: return "P";
      case KEY_Q: return "Q";
      case KEY_R: return "R";
      case KEY_S: return "S";
      case KEY_T: return "T";
      case KEY_U: return "U";
      case KEY_V: return "V";
      case KEY_W: return "W";
      case KEY_X: return "X";
      case KEY_Y: return "Y";
      case KEY_Z: return "Z";
      case KEY_LEFT_BRACKET: return "LEFT_BRACKET";
      case KEY_BACKSLASH: return "BACKSLASH";
      case KEY_RIGHT_BRACKET: return "RIGHT_BRACKET";
      case KEY_GRAVE_ACCENT: return "GRAVE_ACCENT";
      case KEY_WORLD_1: return "WORLD_1";
      case KEY_WORLD_2: return "WORLD_2";
      case KEY_ESCAPE: return "ESCAPE";
      case KEY_ENTER: return "ENTER";
      case KEY_TAB: return "TAB";
      case KEY_BACKSPACE: return "BACKSPACE";
      case KEY_INSERT: return "INSERT";
      case KEY_DELETE: return "DELETE";
      case KEY_RIGHT: return "RIGHT";
      case KEY_LEFT: return "LEFT";
      case KEY_DOWN: return "DOWN";
      case KEY_UP: return "UP";
      case KEY_PAGE_UP: return "PAGE_UP";
      case KEY_PAGE_DOWN: return "PAGE_DOWN";
      case KEY_HOME: return "HOME";
      case KEY_END: return "END";
      case KEY_CAPS_LOCK: return "CAPS_LOCK";
      case KEY_SCROLL_LOCK: return "SCROLL_LOCK";
      case KEY_NUM_LOCK: return "NUM_LOCK";
      case KEY_PRINT_SCREEN: return "PRINT_SCREEN";
      case KEY_PAUSE: return "PAUSE";
      case KEY_F1: return "F1";
      case KEY_F2: return "F2";
      case KEY_F3: return "F3";
      case KEY_F4: return "F4";
      case KEY_F5: return "F5";
      case KEY_F6: return "F6";
      case KEY_F7: return "F7";
      case KEY_F8: return "F8";
      case KEY_F9: return "F9";
      case KEY_F10: return "F10";
      case KEY_F11: return "F11";
      case KEY_F12: return "F12";
      case KEY_F13: return "F13";
      case KEY_F14: return "F14";
      case KEY_F15: return "F15";
      case KEY_F16: return "F16";
      case KEY_F17: return "F17";
      case KEY_F18: return "F18";
      case KEY_F19: return "F19";
      case KEY_F20: return "F20";
      case KEY_F21: return "F21";
      case KEY_F22: return "F22";
      case KEY_F23: return "F23";
      case KEY_F24: return "F24";
      case KEY_F25: return "F25";
      case KEY_KP_0: return "KP_0";
      case KEY_KP_1: return "KP_1";
      case KEY_KP_2: return "KP_2";
      case KEY_KP_3: return "KP_3";
      case KEY_KP_4: return "KP_4";
      case KEY_KP_5: return "KP_5";
      case KEY_KP_6: return "KP_6";
      case KEY_KP_7: return "KP_7";
      case KEY_KP_8: return "KP_8";
      case KEY_KP_9: return "KP_9";
      case KEY_KP_DECIMAL: return "KP_DECIMAL";
      case KEY_KP_DIVIDE: return "KP_DIVIDE";
      case KEY_KP_MULTIPLY: return "KP_MULTIPLY";
      case KEY_KP_SUBTRACT: return "KP_SUBTRACT";
      case KEY_KP_ADD: return "KP_ADD";
      case KEY_KP_ENTER: return "KP_ENTER";
      case KEY_KP_EQUAL: return "KP_EQUAL";
      case KEY_LEFT_SHIFT: return "LEFT_SHIFT";
      case KEY_LEFT_CONTROL: return "LEFT_CONTROL";
      case KEY_LEFT_ALT: return "LEFT_ALT";
      case KEY_LEFT_SUPER: return "LEFT_SUPER";
      case KEY_RIGHT_SHIFT: return "RIGHT_SHIFT";
      case KEY_RIGHT_CONTROL: return "RIGHT_CONTROL";
      case KEY_RIGHT_ALT: return "RIGHT_ALT";
      case KEY_RIGHT_SUPER: return "RIGHT_SUPER";
      case KEY_MENU: return "MENU";
      default: return "unrecognised";
      }
   }

   Key from_glfw_key(int key) {
      switch (key) {
      case GLFW_KEY_SPACE: return KEY_SPACE;
      case GLFW_KEY_APOSTROPHE: return KEY_APOSTROPHE;
      case GLFW_KEY_COMMA: return KEY_COMMA;
      case GLFW_KEY_MINUS: return KEY_MINUS;
      case GLFW_KEY_PERIOD: return KEY_PERIOD;
      case GLFW_KEY_SLASH: return KEY_SLASH;
      case GLFW_KEY_0: return KEY_0;
      case GLFW_KEY_1: return KEY_1;
      case GLFW_KEY_2: return KEY_2;
      case GLFW_KEY_3: return KEY_3;
      case GLFW_KEY_4: return KEY_4;
      case GLFW_KEY_5: return KEY_5;
      case GLFW_KEY_6: return KEY_6;
      case GLFW_KEY_7: return KEY_7;
      case GLFW_KEY_8: return KEY_8;
      case GLFW_KEY_9: return KEY_9;
      case GLFW_KEY_SEMICOLON: return KEY_SEMICOLON;
      case GLFW_KEY_EQUAL: return KEY_EQUAL;
      case GLFW_KEY_A: return KEY_A;
      case GLFW_KEY_B: return KEY_B;
      case GLFW_KEY_C: return KEY_C;
      case GLFW_KEY_D: return KEY_D;
      case GLFW_KEY_E: return KEY_E;
      case GLFW_KEY_F: return KEY_F;
      case GLFW_KEY_G: return KEY_G;
      case GLFW_KEY_H: return KEY_H;
      case GLFW_KEY_I: return KEY_I;
      case GLFW_KEY_J: return KEY_J;
      case GLFW_KEY_K: return KEY_K;
      case GLFW_KEY_L: return KEY_L;
      case GLFW_KEY_M: return KEY_M;
      case GLFW_KEY_N: return KEY_N;
      case GLFW_KEY_O: return KEY_O;
      case GLFW_KEY_P: return KEY_P;
      case GLFW_KEY_Q: return KEY_Q;
      case GLFW_KEY_R: return KEY_R;
      case GLFW_KEY_S: return KEY_S;
      case GLFW_KEY_T: return KEY_T;
      case GLFW_KEY_U: return KEY_U;
      case GLFW_KEY_V: return KEY_V;
      case GLFW_KEY_W: return KEY_W;
      case GLFW_KEY_X: return KEY_X;
      case GLFW_KEY_Y: return KEY_Y;
      case GLFW_KEY_Z: return KEY_Z;
      case GLFW_KEY_LEFT_BRACKET: return KEY_LEFT_BRACKET;
      case GLFW_KEY_BACKSLASH: return KEY_BACKSLASH;
      case GLFW_KEY_RIGHT_BRACKET: return KEY_RIGHT_BRACKET;
      case GLFW_KEY_GRAVE_ACCENT: return KEY_GRAVE_ACCENT;
      case GLFW_KEY_WORLD_1: return KEY_WORLD_1;
      case GLFW_KEY_WORLD_2: return KEY_WORLD_2;
      case GLFW_KEY_ESCAPE: return KEY_ESCAPE;
      case GLFW_KEY_ENTER: return KEY_ENTER;
      case GLFW_KEY_TAB: return KEY_TAB;
      case GLFW_KEY_BACKSPACE: return KEY_BACKSPACE;
      case GLFW_KEY_INSERT: return KEY_INSERT;
      case GLFW_KEY_DELETE: return KEY_DELETE;
      case GLFW_KEY_RIGHT: return KEY_RIGHT;
      case GLFW_KEY_LEFT: return KEY_LEFT;
      case GLFW_KEY_DOWN: return KEY_DOWN;
      case GLFW_KEY_UP: return KEY_UP;
      case GLFW_KEY_PAGE_UP: return KEY_PAGE_UP;
      case GLFW_KEY_PAGE_DOWN: return KEY_PAGE_DOWN;
      case GLFW_KEY_HOME: return KEY_HOME;
      case GLFW_KEY_END: return KEY_END;
      case GLFW_KEY_CAPS_LOCK: return KEY_CAPS_LOCK;
      case GLFW_KEY_SCROLL_LOCK: return KEY_SCROLL_LOCK;
      case GLFW_KEY_NUM_LOCK: return KEY_NUM_LOCK;
      case GLFW_KEY_PRINT_SCREEN: return KEY_PRINT_SCREEN;
      case GLFW_KEY_PAUSE: return KEY_PAUSE;
      case GLFW_KEY_F1: return KEY_F1;
      case GLFW_KEY_F2: return KEY_F2;
      case GLFW_KEY_F3: return KEY_F3;
      case GLFW_KEY_F4: return KEY_F4;
      case GLFW_KEY_F5: return KEY_F5;
      case GLFW_KEY_F6: return KEY_F6;
      case GLFW_KEY_F7: return KEY_F7;
      case GLFW_KEY_F8: return KEY_F8;
      case GLFW_KEY_F9: return KEY_F9;
      case GLFW_KEY_F10: return KEY_F10;
      case GLFW_KEY_F11: return KEY_F11;
      case GLFW_KEY_F12: return KEY_F12;
      case GLFW_KEY_F13: return KEY_F13;
      case GLFW_KEY_F14: return KEY_F14;
      case GLFW_KEY_F15: return KEY_F15;
      case GLFW_KEY_F16: return KEY_F16;
      case GLFW_KEY_F17: return KEY_F17;
      case GLFW_KEY_F18: return KEY_F18;
      case GLFW_KEY_F19: return KEY_F19;
      case GLFW_KEY_F20: return KEY_F20;
      case GLFW_KEY_F21: return KEY_F21;
      case GLFW_KEY_F22: return KEY_F22;
      case GLFW_KEY_F23: return KEY_F23;
      case GLFW_KEY_F24: return KEY_F24;
      case GLFW_KEY_F25: return KEY_F25;
      case GLFW_KEY_KP_0: return KEY_KP_0;
      case GLFW_KEY_KP_1: return KEY_KP_1;
      case GLFW_KEY_KP_2: return KEY_KP_2;
      case GLFW_KEY_KP_3: return KEY_KP_3;
      case GLFW_KEY_KP_4: return KEY_KP_4;
      case GLFW_KEY_KP_5: return KEY_KP_5;
      case GLFW_KEY_KP_6: return KEY_KP_6;
      case GLFW_KEY_KP_7: return KEY_KP_7;
      case GLFW_KEY_KP_8: return KEY_KP_8;
      case GLFW_KEY_KP_9: return KEY_KP_9;
      case GLFW_KEY_KP_DECIMAL: return KEY_KP_DECIMAL;
      case GLFW_KEY_KP_DIVIDE: return KEY_KP_DIVIDE;
      case GLFW_KEY_KP_MULTIPLY: return KEY_KP_MULTIPLY;
      case GLFW_KEY_KP_SUBTRACT: return KEY_KP_SUBTRACT;
      case GLFW_KEY_KP_ADD: return KEY_KP_ADD;
      case GLFW_KEY_KP_ENTER: return KEY_KP_ENTER;
      case GLFW_KEY_KP_EQUAL: return KEY_KP_EQUAL;
      case GLFW_KEY_LEFT_SHIFT: return KEY_LEFT_SHIFT;
      case GLFW_KEY_LEFT_CONTROL: return KEY_LEFT_CONTROL;
      case GLFW_KEY_LEFT_ALT: return KEY_LEFT_ALT;
      case GLFW_KEY_LEFT_SUPER: return KEY_LEFT_SUPER;
      case GLFW_KEY_RIGHT_SHIFT: return KEY_RIGHT_SHIFT;
      case GLFW_KEY_RIGHT_CONTROL: return KEY_RIGHT_CONTROL;
      case GLFW_KEY_RIGHT_ALT: return KEY_RIGHT_ALT;
      case GLFW_KEY_RIGHT_SUPER: return KEY_RIGHT_SUPER;
      case GLFW_KEY_MENU: return KEY_MENU;
      default: return KEY_UNKNOWN;
      }
   }
}
