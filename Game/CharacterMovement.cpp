#include "stdafx.h"
#include "CharacterMovement.h"
#include "Actor.h"
#include "Collider.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "CharacterController.h"

using namespace glm;

/**	On Begin
*******************************************************************************/
void CCharacterMovement::OnBegin()
{
	//--------------------------------------------------- Spawn mesh
	GetActor()->AddComponent<CCollider>();

	// Spawn mesh
	m_Mesh = GetActor()->SpawnActor( "Mesh" );

	{
		m_Material.LoadFromFile( "../Assets/Shader/Deferred/D_default" );

		CMeshRenderer* renderer = m_Mesh->AddComponent<CMeshRenderer>();
		renderer->Set( "../Assets/UnitCube.fbx", 0 );
		renderer->SetMaterial( &m_Material );

		m_Mesh->Transform()->SetScale( vec3( 1.f, 2.f, 1.f ) );
		m_Mesh->Transform()->SetPosition( vec3( 0.f, 1.f, 0.f ) );
	}
}

/**	On Load
*******************************************************************************/
void CCharacterMovement::OnLoad( const CDataStructure & data )
{
	data.Read( "/Character/Movement/Acceleration", m_Acceleration );
	data.Read( "/Character/Movement/Friction", m_Friction );
	data.Read( "/Character/Movement/TurnSpeed", m_TurnSpeed );
}

/**	On Update
*******************************************************************************/
void CCharacterMovement::OnUpdate( const SUpdateInfo & info )
{
	UpdateMovement( info );
	UpdateRotation( info );
}

/**	Update Movement
*******************************************************************************/
void CCharacterMovement::UpdateMovement( const SUpdateInfo & info )
{
	CCharacterController* controller = GetController();
	if ( EnsureNot( !controller ) )
		return;

	vec2 moveInput = controller->GetInputVector();

	if ( moveInput != vec2() )
		moveInput = normalize( moveInput );

	// Physics magic!
	m_Velocity += ( m_Acceleration * moveInput - m_Velocity * m_Friction ) * info.Delta;

	vec3 delta = Math::Make3D( m_Velocity, 0.f ) * info.Delta;

	// Collision checking!
	CCollider* collider = GetActor()->GetComponent<CCollider>();
	if ( collider && delta != vec3() )
	{
		vec3 delta_x = delta * vec3( 1.f, 0.f, 0.f ),
			delta_z = delta * vec3( 0.f, 0.f, 1.f );

		Physics::HitResult result;

		// X axis
		Transform()->AddPosition( delta_x );

		result = collider->QueryCollisions();
		if ( result.hit )
			Transform()->AddPosition( -delta_x );

		// Z axis
		Transform()->AddPosition( delta_z );

		result = collider->QueryCollisions();
		if ( result.hit )
			Transform()->AddPosition( -delta_z );
	}
	else
		Transform()->AddPosition( delta );
}

/**	Update Rotation
*******************************************************************************/
void CCharacterMovement::UpdateRotation( const SUpdateInfo & info )
{
	CCharacterController* controller = GetController();
	if ( EnsureNot( !controller ) )
		return;

	quat targetQuat = angleAxis( controller->GetTargetDirection(), vec3( 0.f, 1.f, 0.f ) );
	quat currentQuat = m_Mesh->Transform()->GetRotation();
	currentQuat = slerp( currentQuat, targetQuat, m_TurnSpeed * info.Delta );

	m_Mesh->Transform()->SetRotation( currentQuat );
}

/**	Get Controller
*******************************************************************************/
CCharacterController* CCharacterMovement::GetController()
{
	if ( m_Controller )
		return m_Controller;

	m_Controller = GetActor()->GetComponent<CCharacterController>();
	return m_Controller;
}
