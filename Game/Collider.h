#pragma once
#include "Component.h"
#include "Physics.h"

class CCollider : public CComponent
{
public:
	CCollider( CActor* actor ) : CComponent( actor ) { Register<CCollider>(); }
	Physics::HitResult QueryCollisions();

	void OnInit() override;
	void OnDestroy() override;

	void SetIsStatic( bool isStatic ) { m_IsStatic = isStatic; }
	bool GetIsStatic() const { return m_IsStatic; }

	void OnTransformCleaned() override;

	Physics::BoundingBox GetBoundingBox();

private:
	bool m_IsStatic = false;
};
