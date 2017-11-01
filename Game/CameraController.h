#pragma once
#include "Component.h"

class CCameraController : public CComponent
{
public:
	CCameraController( CActor* actor ) : CComponent( actor ) { Register<CCameraController>(); }

	void OnLoad( const CDataStructure& data ) override;
	void OnBegin() override;
	void OnPostUpdate( const SUpdateInfo& info ) override;
	void SetPlayer( class CPlayerController* player );

private:
	float m_MoveSpeed = 10.f;
	float m_ZoomSpeed = 15.f;
	float m_AimDistance = 0.5f;
	float m_ZoomBase = 7.f;
	float m_ZoomDistanceFactor = 0.15f;

	class CPlayerController* m_Player = nullptr;
	class CCamera* m_Camera = nullptr;
};