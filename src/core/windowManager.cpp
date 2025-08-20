#include "core/windowManager.hpp"
#include "core/callbacks.hpp"

#include <cstdio>

constexpr int GL_MAJOR{ 3 };
constexpr int GL_MINOR{ 3 };

WindowManager::WindowManager() : window(nullptr) {
  if (!glfwInit()) fprintf(stderr, "Failed to initialize GLFW\n");
}

WindowManager::~WindowManager() {
  destroyWindow();
  glfwTerminate();
}

bool WindowManager::createWindow(const unsigned int width, const unsigned int height, const char* title) {
#ifndef NDEBUG
  fprintf(stderr, "[WindowManager] Create start: %f\n", glfwGetTime());
#endif

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); 

  window = new Window(width, height, title);
  if (!window->getGLFWwindow()) {
    delete window;
    window = nullptr;
    return false;
  }

  glfwSetKeyCallback(window->getGLFWwindow(), key_callback);
  glfwSetFramebufferSizeCallback(window->getGLFWwindow(), framebuffer_size_callback);
  glfwSetScrollCallback(window->getGLFWwindow(), scroll_callback);

  glfwSetInputMode(window->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSwapInterval(1);

#ifndef NDEBUG
  fprintf(stderr, "[WindowManager] Create finish: %f\n", glfwGetTime());
#endif
  return true;
}
void WindowManager::destroyWindow() {
  delete window;
  window = nullptr;
}

void WindowManager::switchActiveWindowVisibiltiy() {
  if (window) {

    if (glfwGetWindowAttrib(getWindow()->getGLFWwindow(), GLFW_VISIBLE))
      glfwHideWindow(getWindow()->getGLFWwindow());
    else 
      glfwShowWindow(getWindow()->getGLFWwindow());
  }
  else {
		fprintf(stderr, "[WindowManager] No active window to switch visibility.\n");
  }
}
