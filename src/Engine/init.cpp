#include "init.hpp"
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

static GLFWwindow* createWindow(const unsigned int width, const unsigned int height, const char* title) {
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) 
		return nullptr;
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enables VSYNC
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return window;
}



GLFWwindow* initGL(unsigned int width, unsigned int height) {
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return nullptr;
	}


#ifndef NDEBUG
	fprintf(stderr, "createWindow Start Time: %f\n", glfwGetTime());
#endif
	GLFWwindow* window = createWindow(width, height, "Mason LEtoile 1146210");
	if (!window) {
		fprintf(stderr, "Failed to initialize window\n");
		return nullptr;
	}
#ifndef NDEBUG
	fprintf(stderr, "createWindow Completed Time: %f\n", glfwGetTime());
#endif
	   



#ifndef NDEBUG
	fprintf(stderr, "gladLoadGLLoader Start Time: %f\n", glfwGetTime());
#endif
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		cleanupWindow(window);
		return nullptr;
	}
#ifndef NDEBUG
	fprintf(stderr, "gladLoadGLLoader Completed Time: %f\n", glfwGetTime());
#endif




	glViewport(0, 0, width, height); 

	return window;
}



