#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in float a_Color;

uniform mat4 u_Camera;
uniform mat4 u_Model;
uniform vec2 u_AimPosition;

out float f_Color;

void main() {
	vec4 world = vec4(0.0, 0.0, 0.0, 1.0);
	world.xz = a_Position * u_AimPosition;

	gl_Position = u_Camera * u_Model * world;
	f_Color = a_Color;
}