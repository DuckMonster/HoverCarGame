#version 330 core

layout (location = 0) out vec3 o_Color;
layout (location = 1) out vec3 o_Normal;
	
void main() {
	//o_Color = vec3(1.0, 0.0, 0.0);
	o_Color = vec3(1.0);
	o_Normal = vec3(0.0, 1.0, 0.0);
}