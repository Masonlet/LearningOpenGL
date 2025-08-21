#pragma once

#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>

class Window {
public:
	Window(const unsigned int width, const unsigned int height, const char* title);
	~Window();

	void pollEvents() const;
	void swapBuffers() const;
	bool shouldClose() const;
	void updateViewport(const unsigned int width, const unsigned int height);

	inline GLFWwindow* getGLFWwindow() const { return window; }
	inline unsigned int getWidth() const { return width; }
	inline unsigned int getHeight() const { return height; }
	inline float getAspect() const { return static_cast<float>(width) / static_cast<float>(height); }

private:
	GLFWwindow* window = nullptr;
	unsigned int width, height;

	bool createWindow(const unsigned int width, const unsigned int height, const char* title);
	bool initGLAD();
	void setupGLState();
};