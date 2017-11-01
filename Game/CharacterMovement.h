#pragma once
#include "Component.h"
#include "Material.h"

class CCharacterController;

class CCharacterMovement : public CComponent
{
public:
	CCharacterMovement( CActor* actor ) : CComponent( actor ) { Register<CCharacterMovement>(); }
	void OnBegin() override;
	void OnLoad( const CDataStructure& data ) override;
	void OnUpdate( const SUpdateInfo& info ) override;

private:
	void UpdateMovement( const SUpdateInfo& info );
	void UpdateRotation( const SUpdateInfo& info );
	CCharacterController* GetController();

	CMaterial m_Material;
	CActor* m_Mesh = nullptr;
	CCharacterController* m_Controller = nullptr;

	//--------------------------------------------------- Stats
	float m_Acceleration = 140.f;
	float m_Friction = 15.f;
	float m_TurnSpeed = 10.f;
	//---------------------------------------------------

	//--------------------------------------------------- Vars

	//---------------------------------------------------

	glm::vec2 m_Velocity;
};
