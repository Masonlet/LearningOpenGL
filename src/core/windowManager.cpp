#include "core/windowManager.hpp"
#include "core/callbacks.hpp"

#include <cstdio>

constexpr int GL_MAJOR{ 3 };
constexpr int GL_MINOR{ 3 };

GLFWwindow* WindowManager::create(int width, int height, const char* title) {
#ifndef NDEBUG
  fprintf(stderr, "[WindowManager] create start: %f\n", glfwGetTime());
#endif

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
  // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Optional
  // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);   // Optional

  GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) return nullptr;

  glfwMakeContextCurrent(window);
  glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);
  glfwSwapInterval(1); // Enable VSYNC
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#ifndef NDEBUG 
  fprintf(stderr, "[WindowManager] create finish: %f\n", glfwGetTime());
#endif
  return window;
}

GLFWwindow* WindowManager::initGL(const unsigned int width, const unsigned int height) {
#ifndef NDEBUG
  fprintf(stderr, "[WindowManager] initGL start: %f\n", glfwGetTime());
#endif

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

#ifndef NDEBUG 
  fprintf(stderr, "[WindowManager] initGL finish: %f\n", glfwGetTime());
#endif
  return window;
}

void WindowManager::setCallbacks(GLFWwindow* window) {
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

void WindowManager::destroy(GLFWwindow* window) {
  if (window) glfwDestroyWindow(window);
}

void WindowManager::cleanupGL(const unsigned int program) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteProgram(program);
  glBindVertexArray(0);
}