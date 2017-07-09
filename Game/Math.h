#pragma once
namespace Math
{
	const float Deg2Rad = glm::pi<float>( ) / 180.f;
	const float Rad2Deg = 180.f / glm::pi<float>( );

	inline glm::quat EulerToQuat( const glm::vec3& euler ) 
	{
		return glm::quat( glm::yawPitchRoll( euler.y * Deg2Rad, euler.x * Deg2Rad, euler.z * Deg2Rad ) );
	}
}