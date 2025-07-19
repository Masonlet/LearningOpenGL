#pragma once 

#include <GLFW/glfw3.h>

#include "math/vec2.hpp"
#include <unordered_map>

class InputManager {
public:
  void Update(GLFWwindow* window);

  bool IsKeyDown(int key) const;
  inline Vec2 GetMouseDelta() const { return mouseDelta;  }

  void SetCursorLocked(bool locked);
  inline bool IsCursorLocked() const { return cursorLocked; }

private:
  std::unordered_map<int, bool> keyState;
  Vec2 mouseDelta{ 0.0f, 0.0f };
  double lastMouseX{ 0.0 }, lastMouseY{ 0.0 };
  bool firstMouse = true;
  bool cursorLocked = true;
};