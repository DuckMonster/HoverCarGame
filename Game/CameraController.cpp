#include "stdafx.h"
#include "CameraController.h"
#include "PlayerController.h"
#include "Transform.h"
#include "Actor.h"
#include "Camera.h"

using namespace glm;

/** On Load
*******************************************************************************/
void CCameraController::OnLoad( const CDataStructure & data )
{
	data.Read( "/Player/Camera/MoveSpeed", m_MoveSpeed );
	data.Read( "/Player/Camera/ZoomSpeed", m_ZoomSpeed );
	data.Read( "/Player/Camera/AimDistance", m_AimDistance );
	data.Read( "/Player/Camera/ZoomBase", m_ZoomBase );
	data.Read( "/Player/Camera/ZoomDistanceFactor", m_ZoomDistanceFactor );
}

/**	On Begin
*******************************************************************************/
void CCameraController::OnBegin()
{
	m_Camera = GetActor()->FindComponent<CCamera>();
	//m_Camera->SetProjection( true );
}

/**	On Post Update
*******************************************************************************/
void CCameraController::OnPostUpdate( const SUpdateInfo & info )
{
	if ( !m_Player )
		return;

	// Lerp position
	vec3 target = m_Player->Transform()->GetPosition();
	
	vec3 position = Transform()->GetPosition();
	position = lerp( position, target, m_MoveSpeed * info.Delta );

	Transform()->SetPosition( position );

	// Lerp zoom
	float zoom = m_Camera->GetOrthoHeight();
	float targetZoom = m_ZoomBase;

	zoom = lerp( zoom, targetZoom, 5.f * info.Delta );
	m_Camera->SetOrthoHeight( zoom );
}

/**	Set Player
*******************************************************************************/
void CCameraController::SetPlayer( CPlayerController* player )
{
	m_Player = player;

	if ( m_Player )
		Transform()->SetTransform( m_Player->Transform() );
}