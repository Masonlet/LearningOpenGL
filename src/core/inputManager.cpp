#include <GLFW/glfw3.h>
#include "utils/log.hpp"
#include "core/inputManager.hpp"

void InputManager::Update(GLFWwindow* window) {
  for (int i = 0; i < TRACKED_KEY_COUNT; ++i) {
    previousKeyState[i] = keyState[i];
    keyState[i] = glfwGetKey(window, trackedKeys[i]) == GLFW_PRESS;
  }
  
  double xPos, yPos;
  glfwGetCursorPos(window, &xPos, &yPos);

  if (!cursorLocked) {
    mouseDelta = { 0.0f, 0.0f };
    lastMouseX = xPos;
    lastMouseY = yPos;
    firstMouse = true;
    return;
  }

  if (firstMouse) {
    lastMouseX = xPos;
    lastMouseY = yPos;
    firstMouse = false;
  }

  mouseDelta.x = static_cast<float>(xPos - lastMouseX);
  mouseDelta.y = static_cast<float>(lastMouseY - yPos);
  lastMouseX = xPos;
  lastMouseY = yPos;
}

bool InputManager::IsKeyDown(int key) const {
  for (int i = 0; i < TRACKED_KEY_COUNT; ++i)
    if (trackedKeys[i] == key)
      return keyState[i];

  return false;
}
bool InputManager::IsKeyPressed(int key) const {
  for (int i = 0; i < TRACKED_KEY_COUNT; ++i)
    if (trackedKeys[i] == key)
      return keyState[i] && !previousKeyState[i]; 

  return false;
}

void InputManager::SetCursorLocked(bool locked) {
  cursorLocked = locked;
  firstMouse = true;
}

