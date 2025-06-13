#version 330 core

//uniform mat4 MVP;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform bool useOverrideColour;
uniform vec3 colourOverride;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vCol;
layout(location = 2) in vec3 vNorm;

out vec4 fragColour;
out vec3 normal;

void main() {
  mat4 MVP = proj * view * model;
  gl_Position = MVP * vec4(vPos, 1.0);

  if(useOverrideColour){
    fragColour = vec4(colourOverride, 1.0);
  } else {
    fragColour = vCol;
  }
  normal = vNorm;
}
