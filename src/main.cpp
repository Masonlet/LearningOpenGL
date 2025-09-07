#include <glad/glad.h>   // Must be included before GLFW, includes the required OpenGL headers
#include <GLFW/glfw3.h>
#include <iostream>

// Branch 0.1 : Bare Minimum OpenGL Window

int main() {
	// Initialize GLFW
	// Must be called before any other GLFW function
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return EXIT_FAILURE;
	}

	// Configure GLFW OpenGL context using glfwWindowHint
	// This first argument is the option to be configured
	// The second argument is the value to set the option to
	// More information can be found at: https://www.glfw.org/docs/latest/window.html#window_hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version 3.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL version x.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// If on Mac OS X Enable forward compatibility by uncommenting the line below
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

	// Create a window object -- Holds all the windowing data, required by most of GLFW's functions
	// glfwCreateWindow(width, height, title, monitor, share)
	// Monitor = NULL -> windowed mode, non-NULL -> full screen mode
	// Share = NULL -> do not share resources with other context, non-NULL -> share resources with the specified context
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearningOpenGL", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Make the context of the window the main context on this thread
	// Makes future OpenGL calls refer to this context
	glfwMakeContextCurrent(window);

	// Load all required OpenGL function pointers using GLAD, must be called after context creation
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return EXIT_FAILURE;
	}

	// Event loop, runs until the window is closed (Clicks X, alt+F4, etc.)
	while (!glfwWindowShouldClose(window)) {
		// Poll inputs/events (keyboard, mouse, window resize, etc.)
		glfwPollEvents();

		// Draws to the back buffer while the front buffer is being displayed
		// Then, swaps front and back buffers at the end of the loop
		glfwSwapBuffers(window); 
	}

	// Destroys the window and its context
	glfwDestroyWindow(window); 
	// Terminates GLFW, clearing any resources allocated by GLFW
	glfwTerminate();
	return EXIT_SUCCESS;
}
