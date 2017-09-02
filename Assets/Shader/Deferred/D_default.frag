#version 330 core

in VS_OUT {
	vec3 normal;
} fs_in;

layout (location = 0) out vec3 o_Color;
layout (location = 1) out vec3 o_Normal;
	
void main() {
	//o_Color = vec3(1.0, 0.0, 0.0);
	o_Color = vec3(1.0);
	o_Normal = fs_in.normal;
}