#pragma once

// OpenGL and GLAD
#define GLFW_INCLUDE_NONE // Disables inclusion of the development environment header to ensure there are no header conflicts
#include <GLFW/glfw3.h> // Include GFLW API's constants, types, and function prototypes. By default also includes OpenGL header

#include <glad.h> // If "GLFW_INCLUDE_NONE" was not defined this would need to be included before glfw3.h

void cleanupWindow(GLFWwindow* window);
void cleanupGL(unsigned int program);

GLFWwindow* initGL(unsigned int width, unsigned int height);



