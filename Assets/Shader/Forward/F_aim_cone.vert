#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in float a_Color;

uniform mat4 u_Camera;
uniform mat4 u_Model;

uniform vec2 u_AimPosition;
uniform float u_Angle;
uniform int u_Resolution;

out float f_Color;

void main() {
	int id = gl_VertexID - 1;
	float angle = float(id) / (float(u_Resolution - 1) / 2.0) - 1.0;

	vec2 p = a_Position * u_AimPosition;
	p = vec2(
		p.x * cos(angle * u_Angle) - p.y * sin(angle * u_Angle),
		p.x * sin(angle * u_Angle) + p.y * cos(angle * u_Angle)
	);

	vec4 vPos = vec4(0.0, 0.0, 0.0, 1.0);
	vPos.xz = p;

	gl_Position = u_Camera * u_Model * vPos;

	f_Color = a_Color;
	if (gl_VertexID == 0)
		f_Color = -(3.0 / length(u_AimPosition));
}