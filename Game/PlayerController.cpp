#include "stdafx.h"
#include "PlayerController.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "Actor.h"
#include "MeshRenderer.h"
#include "Network.h"
#include "CameraController.h"
#include "Projectile.h"
#include "LineRenderer.h"
#include "CustomRenderer.h"
#include "Collider.h"
#include "Random.h"

using namespace Input;
using namespace Math;
using namespace glm;
using namespace Network;

/**	Constructor
*******************************************************************************/
CPlayerController::CPlayerController( CActor* actor ) : CCharacterController( actor )
{
	Register<CPlayerController>();

	RPC_Possess = RegisterRPC( &CPlayerController::NetPossess, RPC_Policy::Peers( nullptr, 0 ) );
	RPC_SetInput = RegisterRPC( &CPlayerController::NetSetInput, RPC_Policy::BroadcastAll() );
}

/**	Destructor
*******************************************************************************/
CPlayerController::~CPlayerController()
{
}

/**	On Init
*******************************************************************************/
void CPlayerController::OnLoad( const CDataStructure& data )
{
}

/**	On Begin
*******************************************************************************/
void CPlayerController::OnBegin()
{
}

/**	On Pre Update
*******************************************************************************/
void CPlayerController::OnPreUpdate( const SUpdateInfo & info )
{
	if ( m_IsLocal )
		UpdateInput( info );
}

/**	Update Input
*******************************************************************************/
void CPlayerController::UpdateInput( const SUpdateInfo & info )
{
#if !defined(GAME_SERVER)
	InputByte prevInput = m_InputByte;

	//--------------------------------------------------- Keyboard Input
	m_InputByte.SetKey( INPUT_Up, info.Input.KeyDown( sf::Keyboard::W ) );
	m_InputByte.SetKey( INPUT_Right, info.Input.KeyDown( sf::Keyboard::D ) );
	m_InputByte.SetKey( INPUT_Down, info.Input.KeyDown( sf::Keyboard::S ) );
	m_InputByte.SetKey( INPUT_Left, info.Input.KeyDown( sf::Keyboard::A ) );

	//--------------------------------------------------- Mouse Input
	float prevDirection = GetTargetDirection();
	{
		CCamera* camera = GetScene()->GetActiveCamera();
		Math::Ray mouseRay = camera->ScreenToWorld( info.Input.MousePosition() );
		Math::Plane aimPlane = Math::Plane::PointNormal( vec3( 0.f, 1.f, 0.f ), vec3( 0.f, 1.f, 0.f ) );

		m_AimPosition = Math::Make2D( mouseRay.Intersect( aimPlane ) ) - Math::Make2D( Transform()->GetPosition() );
	}

	//--------------------------------------------------- Should sync?
	if ( prevInput != m_InputByte || GetTargetDirection() != prevDirection )
	{
		RPC_SetInput->Send( m_InputByte, Transform()->GetPosition(), m_AimPosition );
	}
#endif
}

/**	Get Input Vector
*******************************************************************************/
vec2 CPlayerController::GetInputVector()
{
	vec2 input;

	if ( m_InputByte.GetKey( INPUT_Up ) )
		input.y -= 1.f;
	if ( m_InputByte.GetKey( INPUT_Right ) )
		input.x += 1.f;
	if ( m_InputByte.GetKey( INPUT_Down ) )
		input.y += 1.f;
	if ( m_InputByte.GetKey( INPUT_Left ) )
		input.x -= 1.f;

	return input;
}

#if defined(GAME_SERVER)
/**	Send Possess
*******************************************************************************/
void CPlayerController::SendPossess( neteng::peer_hash hash )
{
	RPC_Possess->SendPolicy( RPC_Policy::Peer( hash ) );
}
#endif

/**	Net Possess
*******************************************************************************/
void CPlayerController::NetPossess()
{
	m_IsLocal = true;

	//--------------------------------------------------- Update Camera
	CCameraController* camera = GetScene()->GetRootActor()->FindComponent<CCameraController>();
	if ( EnsureNot( !camera ) )
		return;

	camera->SetPlayer( this );
}

/**	Net Set Input
*******************************************************************************/
void CPlayerController::NetSetInput( InputByte & byte, glm::vec3 & position, glm::vec2 & aimPosition )
{
	m_InputByte = byte;
	Transform()->SetPosition( position );
	m_AimPosition = aimPosition;
}
