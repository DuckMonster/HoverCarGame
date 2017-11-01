#version 330 core

in vec3 a_Position;

uniform mat4 u_Camera;
uniform mat4 u_Model;

out VS_OUT {
	vec3 world;
} vs_out;

void main() {
	vec4 vert = vec4(a_Position * 50.0, 1.0);

	gl_Position = u_Camera * u_Model * vert;
	vs_out.world = (u_Model * vert).xyz;
}