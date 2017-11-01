#include "stdafx.h"
#include "ComponentFactory.h"

#include "Camera.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "MeshRenderer.h"
#include "DirectionalLight.h"
#include "Collider.h"

#include "ServerBall.h"
#include "CustomRenderer.h"
#include "Projectile.h"
#include "LineRenderer.h"
#include "CharacterController.h"
#include "CharacterMovement.h"
#include "SimpleAIController.h"

component_type_id CComponentFactory::s_NumFactories = 0;
std::vector<ComponentType> CComponentFactory::s_Factories;

#define REGISTER(type) RegisterComponent< ## type ## >(#type)

void CComponentFactory::RegisterAllComponents()
{
	REGISTER( CCamera );
	REGISTER( CMeshRenderer );
	REGISTER( CCustomRenderer );
	REGISTER( CDirectionalLight );
	REGISTER( CCollider );

	REGISTER( CCameraController );
	REGISTER( CCharacterMovement );
	REGISTER( CCharacterController );
	REGISTER( CPlayerController );
	REGISTER( CServerBall );
	REGISTER( CProjectile );
	REGISTER( CLineRenderer );
	REGISTER( CSimpleAIController );
}