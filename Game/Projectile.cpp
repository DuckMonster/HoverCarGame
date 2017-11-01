#include "stdafx.h"
#include "Projectile.h"
#include "Actor.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "LineRenderer.h"
#include "Physics.h"
#include "Scene.h"
#include "Collider.h"
#include "Gizmo.h"
#include "Color.h"

using namespace glm;

/**	On begin
*******************************************************************************/
void CProjectile::OnBegin()
{
	// Spawn mesh
	CActor* mesh = GetActor()->SpawnActor( "Mesh" );

	if ( !Network::IsServer() )
	{
		m_MeshMaterial.LoadFromFile( "../Assets/Shader/Deferred/D_default" );

		mesh->Transform()->SetScale( vec3( 0.5f, 0.5f, 0.5f ) );
		CLineRenderer* lineRenderer = mesh->AddComponent<CLineRenderer>();
		lineRenderer->m_Width = 0.8f;
	}
}

/**	On Load
*******************************************************************************/
void CProjectile::OnLoad( const CDataStructure & data )
{
	data.Read( "/Projectile/Speed", m_Velocity );

	float size;
	data.Read( "/Projectile/Size", size );
	Transform()->SetScale( vec3( size ) );
}

/**	Init Projectile
*******************************************************************************/
void CProjectile::InitProjectile( glm::vec3 position, float angle )
{
	m_Direction = vec2( cos( angle ), -sin( angle ) );
	Transform()->SetPosition( position + vec3( m_Direction.x, 0.f, m_Direction.y ) * 2.f );

	m_StartPosition = position;
}

/**	On Update
*******************************************************************************/
void CProjectile::OnUpdate( const SUpdateInfo & info )
{
	if ( !m_HasHit )
	{
		vec3 origin = Transform()->GetWorldPosition();

		vec2 origin2D = vec2( origin.x, origin.z );
		vec2 delta = m_Direction * m_Velocity * info.Delta;
		Physics::PhysicsRay ray( origin2D, origin2D + delta );

		Physics::HitResult result;

		GetScene()->GetPhysics()->QueryCollision( ray, result );
		if ( result.hit )
		{
			Debug_Log( "Hit \"%s\"", result.actor->GetName() );
			m_HasHit = true;
			Transform()->SetWorldPosition( vec3( result.position.x, origin.y, result.position.y ) );
		}
		else
			Transform()->AddPosition( vec3( delta.x, 0.f, delta.y ) );

		if ( length( Transform()->GetPosition() - m_StartPosition ) >= 100.f )
			m_HasHit = true;
	}
	else
	{
		m_DestroyTimer -= info.Delta;
		if ( m_DestroyTimer <= 0.f )
		{
			GetActor()->Destroy();
		}
	}
}
