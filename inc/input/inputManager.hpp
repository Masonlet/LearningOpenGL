#pragma once 

#include <GLFW/glfw3.h>
#include "starletmath/vec2.hpp"

class InputManager {
public:
  void update(GLFWwindow* window);
    
  bool isKeyDown(int key) const;
  bool isKeyPressed(int key) const;

  inline Vec2 getMouseDelta() const { return mouseDelta;  }

  void setCursorLocked(bool locked);
  inline bool isCursorLocked() const { return cursorLocked; }

private:
  static constexpr  int TRACKED_KEY_COUNT{ 20 };
  static constexpr int trackedKeys[TRACKED_KEY_COUNT] = {
    GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
    GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL,
    GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT,
    GLFW_KEY_COMMA, GLFW_KEY_PERIOD, GLFW_KEY_E, GLFW_KEY_Q,
    GLFW_KEY_R, GLFW_KEY_F, GLFW_KEY_T, GLFW_KEY_G, GLFW_KEY_Y, GLFW_KEY_H
  };

  bool keyState[TRACKED_KEY_COUNT]{false};
  bool previousKeyState[TRACKED_KEY_COUNT]{false};

  Vec2 mouseDelta{ 0.0f, 0.0f };
  double lastMouseX{ 0.0 }, lastMouseY{ 0.0 };
  bool firstMouse{ true }, cursorLocked{ true };
};