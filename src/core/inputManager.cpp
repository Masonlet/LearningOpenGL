#include "core/inputManager.hpp"

void InputManager::Update(GLFWwindow* window) {
  keyState.clear();

  for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) 
    keyState[key] = glfwGetKey(window, key) == GLFW_PRESS;
  
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
  std::unordered_map<int, bool>::const_iterator it = keyState.find(key);
  return it != keyState.end() && it->second;
}

void InputManager::SetCursorLocked(bool locked) {
  cursorLocked = locked;
  firstMouse = true;
}

