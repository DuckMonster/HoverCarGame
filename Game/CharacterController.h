#pragma once
#include "Component.h"

class CCharacterController : public CComponent
{
public:
	CCharacterController( CActor* actor ) : CComponent( actor ) { Register<CCharacterController>(); }

	virtual float GetTargetDirection() { return 0.f; }
	virtual glm::vec2 GetInputVector() { return glm::vec2(); }

private:
};