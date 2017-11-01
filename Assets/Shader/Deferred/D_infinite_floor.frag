#version 330 core

const vec3 COL_1 = vec3(0.2, 0.2, 0.2);
const vec3 COL_2 = vec3(0.16, 0.16, 0.16);

in VS_OUT {
	vec3 world;
} fs_in;

layout (location = 0) out vec3 o_Color;
layout (location = 1) out vec3 o_Normal;

void main() {
	float total = floor(fs_in.world.x) + floor(fs_in.world.z);
	total = mod(total, 2.0);
	o_Color = mix(COL_1, COL_2, step(total, 0.5));
	o_Normal = vec3(0.0, 1.0, 0.0);
}