#include "stdafx.h"
#include "PhysicsMath.h"

using namespace glm;

namespace Physics
{
	/**	AABB - AABB intersection
	*******************************************************************************/
	bool Intersect( const BoundingBox& first, const BoundingBox& second )
	{
		return first.min.x < second.max.x &&
			second.min.x < first.max.x &&
			first.min.y < second.max.y &&
			second.min.y < first.max.y;
	}

	/**	AABB - Ray intersection
	*******************************************************************************/
	bool Intersect( const BoundingBox& box, const PhysicsRay& ray, glm::vec2& outPosition, glm::vec2& outNormal )
	{
		Face2D face;

		{
			face.first = vec2( box.max.x, box.min.y );
			face.second = box.max;

			if ( Intersect( face, ray, outPosition, outNormal ) )
				return true;
		}
		{
			face.first = box.min;
			face.second = vec2( box.max.x, box.min.y );

			if ( Intersect( face, ray, outPosition, outNormal ) )
				return true;
		}
		{
			face.first = vec2( box.min.x, box.max.y );
			face.second = box.min;

			if ( Intersect( face, ray, outPosition, outNormal ) )
				return true;
		}
		{
			face.first = box.max;
			face.second = vec2( box.min.x, box.max.y );

			if ( Intersect( face, ray, outPosition, outNormal ) )
				return true;
		}

		return false;
	}

	/**	Face2D - Ray intersection
	*******************************************************************************/
	bool Intersect( const Face2D& face, const PhysicsRay& ray, glm::vec2& outPosition, glm::vec2& outNormal )
	{
		glm::vec2 faceNormal = face.normal();

		float len = dot( faceNormal, ray.origin ) - dot( faceNormal, face.first );
		float dir = dot( -faceNormal, ray.direction );

		// Facing the wrong way
		if ( dir * sign( len ) < 0.f )
			return false;

		// Ray length to hit face is longer than maxrange
		if ( len / dir > ray.maxLength )
			return false;

		// Intersect position
		vec2 hitPos = ray.origin + ray.direction * ( len / dir );
		outPosition = hitPos;
		outNormal = faceNormal;

		// Get tangent position of ray, and both vertices, see if the ray is inbetween the vertex tangents
		vec2 tangent = face.tangent();
		float firstTan = dot( tangent, face.first ),
			secondTan = dot( tangent, face.second ),
			rayTan = dot( tangent, hitPos );

		return firstTan <= rayTan && rayTan <= secondTan;
	}
}