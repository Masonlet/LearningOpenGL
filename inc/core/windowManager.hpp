#pragma once
  
// OpenGL and GLAD
#define GLFW_INCLUDE_NONE // Disables inclusion of the development environment header to ensure there are no header conflicts
#include <GLFW/glfw3.h> // Include GFLW API's constants, types, and function prototypes. By default also includes OpenGL header
#include <glad/glad.h> // If "GLFW_INCLUDE_NONE" was not defined this would need to be included before glfw3.h

class WindowManager {
public:
  static GLFWwindow* create(int width, int height, const char* title);

  static void setCallbacks(GLFWwindow* window);
  static GLFWwindow* initGL(const unsigned int width, const unsigned int height);
  static void setupGLState();

  static void destroy(GLFWwindow* window);
  static void cleanupGL(unsigned int program);
};