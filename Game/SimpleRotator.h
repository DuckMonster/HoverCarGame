#pragma once
#include "Component.h"

class CSimpleRotator : public CComponent
{
public:
	CSimpleRotator( class CActor* actor );
	void OnUpdate( const SUpdateData& data ) override;

public:
	glm::vec3	m_Angle = glm::vec3( 0.f, 1.f, 0.f );
	float		m_Speed = 1.f;
};