#version 330 core

in VS_OUT {
	vec3 normal;
	vec2 uv;
} fs_in;

uniform sampler2D u_Sampler;
out vec4 o_Color;

void main() {
	o_Color = texture(u_Sampler, fs_in.uv);
}