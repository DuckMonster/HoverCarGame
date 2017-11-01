#pragma once
#include "Component.h"

class CDirectionalLight : public CComponent
{
public:
	CDirectionalLight( CActor* actor ) : CComponent( actor ) { Register<CDirectionalLight>(); }
};