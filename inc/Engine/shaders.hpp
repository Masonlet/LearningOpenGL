#pragma once

// OpenGL and GLAD
#define GLFW_INCLUDE_NONE // Disables inclusion of the development environment header to ensure there are no header conflicts
#include <GLFW/glfw3.h> // Include GFLW API's constants, types, and function prototypes. By default also includes OpenGL header

#include <glad/glad.h> // If "GLFW_INCLUDE_NONE" was not defined this would need to be included before glfw3.h

inline constexpr const char* VERTEX_SHADER_SOURCE = R"(
	#version 330 core
	uniform mat4 MVP;

	layout(location = 0) in vec3 inPos;
	layout(location = 1) in vec3 inNorm;
	layout(location = 2) in vec3 inCol;

	out vec3 fragColour;

	void main() {
		gl_Position = MVP * vec4(inPos, 1.0);
		fragColour = inCol;
	}
)";
inline constexpr const char* FRAGMENT_SHADER_SOURCE = R"(
	#version 330 core

	in vec3 fragColour;

	layout(location = 0) out vec4 outColour;

	void main() {
		outColour = vec4(fragColour, 1.0);
	}
)";

unsigned int initializeShaders();

unsigned int createBuffer(const unsigned int type, const unsigned int SizeOfVertexArrayInBytes, const void* data);
unsigned int createArray(const unsigned int vbo, const unsigned int ebo);