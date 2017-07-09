#version 330 core

in vec3 a_Position;
in vec3 a_Normal;
in vec2 a_TexCoord;

uniform mat4 u_Camera;
uniform mat4 u_Model;

out VS_OUT {
	vec3 normal;
	vec2 uv;
} vs_out;

void main() {
	gl_Position = u_Camera * u_Model * vec4(a_Position, 1.0);
	vs_out.normal = normalize((u_Model * vec4(a_Normal, 0.0)).xyz);
	vs_out.uv = a_TexCoord;
}