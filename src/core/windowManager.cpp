#include "core/windowManager.hpp"
#include "core/callbacks.hpp"

#include <cstdio>

GLFWwindow* WindowManager::create(int width, int height, const char* title) {
#ifndef NDEBUG
  fprintf(stderr, "[WindowManager] create() start: %f\n", glfwGetTime());
#endif

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Optional
  // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);   // Optional

  GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) return nullptr;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable VSYNC
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#ifndef NDEBUG
  fprintf(stderr, "[WindowManager] create() finish: %f\n", glfwGetTime());
#endif

  return window;
}

void WindowManager::setCallbacks(GLFWwindow* window) {
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void WindowManager::destroy(GLFWwindow* window) {
  if (window) glfwDestroyWindow(window);
}

GLFWwindow* WindowManager::initGL(const unsigned int width, const unsigned int height) {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return nullptr;
  }

  GLFWwindow* window = create(width, height, "Mason LEtoile 1146210");
  if (!window) {
    fprintf(stderr, "Failed to create window\n");
    return nullptr;
  }

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    fprintf(stderr, "[WindowManager ERROR] Failed to initialize GLAD\n");
    destroy(window);
    return nullptr;
  }

  glViewport(0, 0, width, height);
  return window;
}

void WindowManager::setupGLState() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //glEnable(GL_CULL_FACE);
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void WindowManager::cleanupGL(const unsigned int program) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteProgram(program);
  glBindVertexArray(0);
}