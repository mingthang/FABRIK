#version 460 core

layout(location = 0) in vec2 aPos;

uniform mat4 u_model;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_model * vec4(aPos, 0.0, 1.0);
}