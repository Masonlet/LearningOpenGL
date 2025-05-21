#include "shaders.hpp"
#include "files.hpp"
#include "Vertex.hpp"

#include <cstdio>

unsigned int createBuffer(const unsigned int type, const unsigned int SizeOfVertexArrayInBytes, const void* data) {
	unsigned int buffer{0};

    glGenBuffers(1, &buffer); //Generate Vertex Buffer Object (VBO)
    glBindBuffer(type, buffer); //Bind VBO
    glBufferData(type, SizeOfVertexArrayInBytes, data, GL_STATIC_DRAW);

	return buffer;
}
unsigned int createArray(const unsigned int vbo, const unsigned int ebo) {
	unsigned int vao; //Vertex Array Object (VAO)

    glGenVertexArrays(1, &vao); //Generate VAO
    glBindVertexArray(vao); //Bind VAO

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, col)));
	
	glBindVertexArray(0);
	return vao;
}
static unsigned int createShader(const unsigned int type, const char* text) {
	const unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);

	int success{0};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		fprintf(stderr, "Shader compilation failed: %s\n", log);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

static unsigned int createProgram(const unsigned int vertex_shader, const unsigned int fragment_shader) {
	const unsigned int program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	int success{0};
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {
		char log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		fprintf(stderr, "Program linking failed: %s\n", log);
		glDeleteProgram(program);
		return 0;
	}

	return program;
}

unsigned int initializeShaders() {
	const unsigned int vertex_shader = createShader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
	const unsigned int fragment_shader = createShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);
	if (vertex_shader == 0 || fragment_shader == 0)
		return 0;

	unsigned int program = createProgram(vertex_shader, fragment_shader);
	return program;
}