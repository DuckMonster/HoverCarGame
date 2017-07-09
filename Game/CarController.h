#pragma once
#include "Component.h"

class CCarController : public CComponent
{
public:
	CCarController( class CActor* actor );
	void OnUpdate( const SUpdateData& data ) override;

private:
};