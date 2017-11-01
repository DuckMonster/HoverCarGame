#pragma once

namespace Physics
{
	/**	Bounding Box
	*******************************************************************************/
	struct BoundingBox
	{
		glm::vec2 min;
		glm::vec2 max;
	};

	/**	Ray
	*******************************************************************************/
	struct PhysicsRay
	{
		PhysicsRay() {}
		PhysicsRay( const glm::vec2& origin, const glm::vec2& direction, float maxLength )
			: origin( origin ), direction( direction ), maxLength( maxLength ) {}
		PhysicsRay( const glm::vec2& origin, const glm::vec2& target )
			: origin( origin ), direction( normalize( target - origin ) ), maxLength( length( target - origin ) ) {}

		glm::vec2 origin;
		glm::vec2 direction;
		float maxLength = 0.f;
	};

	/**	Face
	*******************************************************************************/
	struct Face2D
	{
		Face2D() : first(), second() {}
		Face2D( glm::vec2 first, glm::vec2 second ) :
			first( first ), second( second ) {}

		glm::vec2 first;
		glm::vec2 second;

		glm::vec2 normal() const
		{
			glm::vec2 norm = second - first;
			norm = normalize( norm );

			return glm::vec2( norm.y, -norm.x );
		}

		glm::vec2 tangent() const
		{
			return normalize( second - first );
		}
	};

	/**	Intersection
	*******************************************************************************/
	bool Intersect( const BoundingBox& box, const BoundingBox& second );
	bool Intersect( const BoundingBox& box, const PhysicsRay& ray, glm::vec2& outPosition, glm::vec2& outNormal );
	bool Intersect( const Face2D& face, const PhysicsRay& ray, glm::vec2& outPosition, glm::vec2& outNormal );
}