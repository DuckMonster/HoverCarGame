#version 330 core

const vec4 COL_1 = vec4(0.2, 0.2, 0.2, 1.0);
const vec4 COL_2 = vec4(0.16, 0.16, 0.16, 1.0);

in VS_OUT {
	vec3 world;
} fs_in;

out vec4 o_Color;

void main() {
	float total = floor(fs_in.world.x) + floor(fs_in.world.z);
	total = mod(total, 2.0);
	o_Color = mix(COL_1, COL_2, step(total, 0.5));
}