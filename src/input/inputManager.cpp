#include <GLFW/glfw3.h>
#include "starletparsers/utils/log.hpp"
#include "input/inputManager.hpp"

void InputManager::update(GLFWwindow* window) {
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

bool InputManager::isKeyDown(int key) const {
  for (int i = 0; i < TRACKED_KEY_COUNT; ++i)
    if (trackedKeys[i] == key)
      return keyState[i];

  return false;
}
bool InputManager::isKeyPressed(int key) const {
  for (int i = 0; i < TRACKED_KEY_COUNT; ++i)
    if (trackedKeys[i] == key)
      return keyState[i] && !previousKeyState[i]; 

  return false;
}

void InputManager::setCursorLocked(bool locked) {
  cursorLocked = locked;
  firstMouse = true;
}

