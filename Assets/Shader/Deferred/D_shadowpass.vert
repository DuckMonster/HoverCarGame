#version 330 core
in vec3 a_Position;

uniform mat4 u_Camera;
uniform mat4 u_Model;

void main() {
	gl_Position = u_Camera * u_Model * vec4(a_Position, 1.0);
}