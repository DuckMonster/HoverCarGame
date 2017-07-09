#pragma once
#include "Component.h"

class CCameraController : public CComponent
{
public:
	CCameraController( class CActor* actor );

	void OnInit( ) override;
	void OnUpdate( const SUpdateData& data ) override;

public:
	class CTransform*	m_Target = nullptr;
};