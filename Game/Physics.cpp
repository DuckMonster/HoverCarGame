#include "stdafx.h"
#include "Physics.h"
#include "Actor.h"
#include "Collider.h"

namespace Physics
{
	/**	Set Root
	*******************************************************************************/
	void CPhysics::SetRoot( CActor * root )
	{
		m_RootActor = root;
		MakeDirty();
	}

	/**	Query Collision
	*******************************************************************************/
	bool CPhysics::QueryCollision( CCollider* other, HitResult& outResult )
	{
		Clean();
		BoundingBox box = other->GetBoundingBox();

		// Check statics
		for ( const StaticBakedCollider& collider : m_StaticColliders )
		{
			if ( collider.collider == other )
				continue;

			if ( Intersect( box, collider.box))
			{
				outResult.hit = true;
				outResult.component = collider.collider;
				outResult.actor = collider.collider->GetActor();

				return true;
			}
		}

		// Check dynamics
		for ( CCollider* collider : m_DynamicColliders )
		{
			if ( collider == other )
				continue;

			if ( Intersect( box, collider->GetBoundingBox() ) )
			{
				outResult.hit = true;
				outResult.component = collider;
				outResult.actor = collider->GetActor();

				return true;
			}
		}

		return false;
	}

	/**	Query Collision (Ray)
	*******************************************************************************/
	bool CPhysics::QueryCollision( const PhysicsRay & ray, HitResult& outResult )
	{
		Clean();

		// Check statics
		for ( const StaticBakedCollider& collider : m_StaticColliders )
		{
			if ( Intersect( collider.box, ray, outResult.position, outResult.normal ) )
			{
				outResult.hit = true;
				outResult.component = collider.collider;
				outResult.actor = collider.collider->GetActor();

				return true;
			}
		}

		// Check dynamics
		for ( CCollider* collider : m_DynamicColliders )
		{
			if ( Intersect( collider->GetBoundingBox(), ray, outResult.position, outResult.normal ) )
			{
				outResult.hit = true;
				outResult.component = collider;
				outResult.actor = collider->GetActor();

				return true;
			}
		}

		return false;
	}

	/**	Clean
	*******************************************************************************/
	void CPhysics::Clean()
	{
		if ( !m_IsDirty )
			return;

		m_AllColliders.clear();
		m_StaticColliders.clear();
		m_DynamicColliders.clear();

		m_RootActor->CollectComponents<CCollider>( m_AllColliders );

		for ( CCollider* collider : m_AllColliders )
		{
			// Static collider
			if ( collider->GetIsStatic() )
			{
				StaticBakedCollider bake;
				bake.collider = collider;
				bake.box = collider->GetBoundingBox();

				m_StaticColliders.push_back( bake );
			}
			// Dynamic collider
			else
			{
				m_DynamicColliders.push_back( collider );
			}
		}

		Debug_Log( "Physics Cleaned" );
		m_IsDirty = false;
	}
}