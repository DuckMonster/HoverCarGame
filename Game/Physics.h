#pragma once
#include "PhysicsMath.h"

class CActor;
class CCollider;

namespace Physics
{
	struct HitResult
	{
		bool hit = false;
		CCollider* component = nullptr;
		CActor* actor = nullptr;
		glm::vec2 position;
		glm::vec2 normal;
	};
	
	class CPhysics
	{
		struct StaticBakedCollider
		{
			CCollider* collider;
			BoundingBox box;
		};

	public:
		CPhysics() {}

		void SetRoot( CActor* root );

		void MakeDirty() { m_IsDirty = true; }
		bool QueryCollision( CCollider* other, HitResult& outResult );
		bool QueryCollision( const PhysicsRay& ray, HitResult& outResult );

	private:
		void Clean();

		CActor* m_RootActor = nullptr;
		bool m_IsDirty = true;

		std::vector<CCollider*> m_AllColliders;
		std::vector<StaticBakedCollider> m_StaticColliders;
		std::vector<CCollider*> m_DynamicColliders;
	};
}