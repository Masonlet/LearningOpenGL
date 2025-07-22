#include <GLFW/glfw3.h>

#include "core/inputManager.hpp"

void InputManager::Update(GLFWwindow* window) {
  for (int i = 0; i < TRACKED_KEY_COUNT; ++i)
    keyState[i] = glfwGetKey(window, trackedKeys[i]) == GLFW_PRESS;
  
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (!cursorLocked) {
    mouseDelta = { 0.0f, 0.0f };
    lastMouseX = xpos;
    lastMouseY = ypos;
    firstMouse = true;
    return;
  }

  if (firstMouse) {
    lastMouseX = xpos;
    lastMouseY = ypos;
    firstMouse = false;
  }

  mouseDelta.x = static_cast<float>(xpos - lastMouseX);
  mouseDelta.y = static_cast<float>(lastMouseY - ypos);

  lastMouseX = xpos;
  lastMouseY = ypos;
}

bool InputManager::IsKeyDown(int key) const {
  for (int i = 0; i < TRACKED_KEY_COUNT; ++i)
    if (trackedKeys[i] == key)
      return keyState[i];

  return false;
}

void InputManager::SetCursorLocked(bool locked) {
  cursorLocked = locked;
  firstMouse = true;
}

