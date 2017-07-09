#version 330 core

in VS_OUT {
	vec3 normal;
} fs_in;

out vec4 o_Color;

void main() {
	o_Color = vec4(fs_in.normal, 1.0);
}