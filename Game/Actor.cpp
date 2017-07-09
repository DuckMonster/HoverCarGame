#include "stdafx.h"
#include "Actor.h"
#include "Scene.h"
#include "Component.h"
#include "Transform.h"
#include "Color.h"
#include "Gizmo.h"

using namespace std;

/**	Constructor
*******************************************************************************/
CActor::CActor( CScene* scene ) :
	m_Scene( scene ),
	m_Game( scene->GetGame( ) )
{
	m_Transform = new CTransform( this );
}

/**	Init
*******************************************************************************/
void CActor::Init( )
{
	Print_Log( "%s created!", m_Name.c_str( ) );
}

/**	Destroy
*******************************************************************************/
void CActor::Destroy( )
{
	for (size_t i = 0; i < NumChildren( ); i++)
	{
		CActor* child = GetChild( i );
		child->Destroy( );
		delete child;
	}

	for (size_t i = 0; i < m_Components.size( ); i++)
	{
		m_Components[i]->OnDestroy( );
		delete m_Components[i];
	}

	m_Transform->SetParent( nullptr );
	delete m_Transform;
	m_Transform = nullptr;
}

/**	Update
*******************************************************************************/
void CActor::Update( const SUpdateData& data )
{
	for (size_t i = 0; i < m_Components.size( ); i++)
		m_Components[i]->OnUpdate( data );

	for (size_t i = 0; i < NumChildren( ); i++)
		GetChild( i )->Update( data );
}

/**	Render
*******************************************************************************/
void CActor::Render( const SRenderData& data )
{
	for (size_t i = 0; i < m_Components.size( ); i++)
		m_Components[i]->OnRender( data );

	for (size_t i = 0; i < NumChildren( ); i++)
		GetChild( i )->Render( data);

	// Render Transform
	glm::vec3 position = Transform( )->GetPosition( ),
		forward = Transform( )->GetForward( ),
		right = Transform( )->GetRight( ),
		up = Transform( )->GetUp( );

	CGizmo::DrawLine( position, position + forward, CColor::Red, 2.f, data.CameraMatrix );
	CGizmo::DrawLine( position, position + up, CColor::Green, 2.f, data.CameraMatrix );
	CGizmo::DrawLine( position, position + right, CColor::Blue, 2.f, data.CameraMatrix );

	for (size_t i = 0; i < NumChildren( ); i++)
	{
		CGizmo::DrawLine( position, GetChild( i )->Transform( )->GetPosition( ),
			CColor::White, 1.f, 
			data.CameraMatrix
		);
	}
}

/**	Num Children
*******************************************************************************/
size_t CActor::NumChildren( )
{
	return m_Transform->NumChildren( );
}

/**	Get Parent
*******************************************************************************/
CActor* CActor::GetParent( )
{
	return m_Transform->GetParent( )->GetActor( );
}

/**	Get Child
*******************************************************************************/
CActor* CActor::GetChild( size_t index )
{
	return m_Transform->GetChild( index )->GetActor( );
}

/**	Spawn Actor
*******************************************************************************/
CActor* CActor::SpawnActor( const string& name, CActor* parent/* = nullptr*/ )
{
	if (parent == nullptr)
		parent = this;

	CActor* newActor = new CActor( m_Scene );
	newActor->SetName( name.length() > 0 ? name : parent->GetName() + "_copy" );
	if (parent)
		newActor->m_Transform->SetParent( parent->m_Transform );

	newActor->Init( );

	return newActor;
}

/**	Remove Component
*******************************************************************************/
bool CActor::RemoveComponent( CComponent* component )
{
	// Find specific component
	vector<CComponent*>::iterator it = find( m_Components.begin( ), m_Components.end( ), component );
	if (it == m_Components.end( ))
		return false;

	// Call destroy event
	component->OnDestroy( );
	delete component;

	m_Components.erase( it );
	return true;
}
