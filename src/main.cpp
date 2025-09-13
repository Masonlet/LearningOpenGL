#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

// Branch 0.15 : Bare Minimum OpenGL Window w/ functions

static bool initializeGLFW() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return true;
}

static GLFWwindow* createWindow(const unsigned int width, const unsigned int height, const char* title) {
	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	return window;
}

static bool initializeGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	return true;
}

int main() {
	if (!initializeGLFW()) return EXIT_FAILURE;

	GLFWwindow* window = createWindow(800, 600, "LearningOpenGL");
	if (!window) return EXIT_FAILURE;

	if (!initializeGlad()) return EXIT_FAILURE;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glfwSwapBuffers(window); 
	}

	glfwDestroyWindow(window); 
	glfwTerminate();
	return EXIT_SUCCESS;
}
