#version 330 core
in vec2 f_UV;

const vec3 TOP_COLOR = vec3(0.7, 0.85, 1.0);
const vec3 BOTTOM_COLOR = vec3(0.2, 1.0, 0.6);

struct Deferred 
{
	sampler2D Color;
	sampler2D Normal;
	sampler2D Depth;
	sampler2D Shadow;
};

uniform Deferred u_Deferred;
uniform vec3 u_LightDirection;
uniform mat4 u_LightSpace;

uniform mat4 u_ProjectionInv;
uniform mat4 u_ViewInv;

out vec4 o_Color;

float Diffuse(in vec3 normal, in vec3 light) 
{
	float diffuse = dot(normal, -light);
	return diffuse * step(0.0, diffuse);
}

vec3 Ambient(in vec3 normal)
{
	float up = dot(normal, vec3(0.0, 1.0, 0.0)) / 2.0 + 0.5;
	return mix(BOTTOM_COLOR, TOP_COLOR, up) * 0.15;
}

float Shadow(in vec3 normal, in float depth) 
{
	// NDC space
	vec4 worldPos = vec4(f_UV.x, f_UV.y, depth, 1.0);

	// [0, 1] -> [-1, 1]
	worldPos.xyz = worldPos.xyz * 2.0 - 1.0;

	// View space
	worldPos = u_ProjectionInv * worldPos;
	worldPos /= worldPos.w; // Perspective divide

	// World Space
	worldPos = u_ViewInv * worldPos;
	worldPos.xyz += normal * 0.1 + 0.0016; // Add bias for shadow mapping

	// Into light space
	vec4 lightNDC = u_LightSpace * worldPos;
	lightNDC /= lightNDC.w; 

	// [-1, 1] -> [0, 1]
	lightNDC = (lightNDC + 1.0) / 2.0;

	float depth_light = texture(u_Deferred.Shadow, lightNDC.xy).x;
	float depth_frag = lightNDC.z;

	// Finally, shadows
	return step(depth_frag, depth_light);
}

void main()
{
	vec3 normal = normalize(texture(u_Deferred.Normal, f_UV).xyz);
	vec3 color = texture(u_Deferred.Color, f_UV).xyz;
	float depth = texture(u_Deferred.Depth, f_UV).x;
	vec3 light = normalize(u_LightDirection);

	float diffuse = Diffuse(normal, light);

	// Ambient
	vec3 ambient = Ambient(normal);

	// Shadow
	float shadow = Shadow(normal, depth);

	o_Color.xyz = color * (diffuse * shadow) + ambient;
	o_Color.a = 1.0;
	//o_Color = ambient;

	//Normalize color
	//float div = max(max(max(o_Color.x, o_Color.y), o_Color.z), 1.0);
	//o_Color /= div;
	//o_Color = Ambient(normal);
}