#version 330 core

uniform sampler2D u_Sampler;

uniform mat4 u_InvProjection;
uniform mat4 u_InvView;

in vec2 f_UV;
out vec3 o_Color;

void main() 
{
	vec4 clipSpace = vec4(0.0, 0.0, 0.0, 1.0);
	clipSpace.xy = f_UV;
	clipSpace.z = texture(u_Sampler, f_UV).x;

	// [0, 1] to [-1, 1]
	clipSpace = clipSpace * 2.0 - 1.0;

	// Viewspace
	clipSpace = u_InvProjection * clipSpace;
	clipSpace /= clipSpace.w; // Perspective divide

	// World space
	clipSpace = u_InvView * clipSpace;

	o_Color = clipSpace.xyz;
	//o_Color = vec3(texture(u_Sampler, f_UV).x);
	//o_Color = vec3(f_UV, 0.0);
}