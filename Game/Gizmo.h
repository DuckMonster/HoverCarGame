#pragma once
class CGizmo
{
public:
	static void InitGizmos( );
	static void DestroyGizmos( );

	static void DrawLine( const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float size, const glm::mat4& camera );
};