#version 330 core

uniform mat4 MVP;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vCol;
layout(location = 2) in vec3 vNorm;

out vec3 color;
out vec3 normal;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    color = vCol;
	normal = vNorm;
}