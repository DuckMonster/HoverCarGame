#pragma once
#include "Component.h"
#include "Material.h"

class CProjectile : public CComponent
{
public:
	CProjectile( CActor* actor ) : CComponent( actor ) { Register<CProjectile>(); }

	void OnBegin() override;
	void OnLoad( const CDataStructure& data ) override;
	void InitProjectile( glm::vec3 position, float angle );

	void OnUpdate( const SUpdateInfo& info ) override;

private:
	bool m_HasHit = false;
	float m_DestroyTimer = 3.f;

	glm::vec3 m_StartPosition;

	glm::vec2 m_Direction;
	float m_Velocity;
	CMaterial m_MeshMaterial;
};