#include "stdafx.h"
#include "Collider.h"
#include "Transform.h"
#include "Actor.h"
#include "Scene.h"

using namespace std;
using namespace glm;

/**	Query Collisions
*******************************************************************************/
Physics::HitResult CCollider::QueryCollisions()
{
	Physics::HitResult result;
	GetScene()->GetPhysics()->QueryCollision( this, result );

	return result;
}

/**	On Init
*******************************************************************************/
void CCollider::OnInit()
{
	GetScene()->GetPhysics()->MakeDirty();
}

/**	On Destroy
*******************************************************************************/
void CCollider::OnDestroy()
{
	GetScene()->GetPhysics()->MakeDirty();
}

/**	On Transform Cleaned
*******************************************************************************/
void CCollider::OnTransformCleaned()
{
	if ( m_IsStatic )
		GetScene()->GetPhysics()->MakeDirty();
}

/**	Get Bounding Box
*******************************************************************************/
Physics::BoundingBox CCollider::GetBoundingBox()
{
	Physics::BoundingBox box;
	quat rotation = Transform()->GetRotation();
	vec3 scale = Transform()->GetWorldScale();

	scale = (mat3)rotation * scale;

	vec3 position = Transform()->GetWorldPosition();

	vec2 position2d( position.x, position.z );
	vec2 halfScale2d( abs( scale.x / 2.f ), abs( scale.z / 2.f ) );

	box.min = position2d - halfScale2d;
	box.max = position2d + halfScale2d;

	return box;
}
