#version 330 core

uniform float u_CritValue;

in float f_Color;
out vec4 o_Color;
	
void main() {
	o_Color = vec4(mix(vec3(0.4), vec3(1.0), u_CritValue), max(f_Color, u_CritValue));
}