#include "init.hpp"
#include "callbacks.hpp"
#include <cstdio>

constexpr int GL_MAJOR{3};
constexpr int GL_MINOR{3};

void cleanupWindow(GLFWwindow* window) {
	if (window) glfwDestroyWindow(window);
}
void cleanupGL(const unsigned int program) {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteProgram(program);
	glUseProgram(0);
	glBindVertexArray(0);
}

static GLFWwindow* createWindow(const int& width, const int& height, const char* title) {
#ifndef NDEBUG
	fprintf(stderr, "createWindow Start Time: %f\n", glfwGetTime());
#endif

	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) 
		return nullptr;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enables VSYNC
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


#ifndef NDEBUG
	fprintf(stderr, "createWindow Finish Time: %f\n", glfwGetTime());
#endif
	return window;
}

GLFWwindow* initGL(unsigned int width, unsigned int height) {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GFLW\n");
		return nullptr;
	}

	GLFWwindow* window = createWindow(width, height, "Mason LEtoile 1146210");
	if (!window) {
		return nullptr;
		fprintf(stderr, "Failed to initialize window\n");
	}

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		cleanupWindow(window);
		return nullptr;
	}

	glViewport(0, 0, width, height);
	return window;
}

