#version 330 core

in float f_Color;
out vec4 o_Color;
	
void main() {
	o_Color.rgb = vec3(1.0);
	o_Color.a = f_Color;
}