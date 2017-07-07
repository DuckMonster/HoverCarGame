#include "stdafx.h"
#include "Actor.h"
#include "Scene.h"

using namespace std;

CActor::CActor( CScene * scene ) :
	m_Scene( scene )
{
}

void CActor::Init( )
{
	std::cout << m_Name << " spawned!\n";
}

void CActor::Destroy( )
{
}

void CActor::Update( float delta )
{
	// do stuff
}

void CActor::Render( float delta )
{
}

void CActor::SetParent( CActor* target )
{
	// Same parent
	if (m_Parent == target)
		return;

	// Parent to root
	if (target == nullptr)
		target = m_Scene->GetRootActor( );

	// Unparent from current
	if (m_Parent)
	{
		actor_iterator foundIt = find( m_Parent->m_Children.begin( ), m_Parent->m_Children.end( ), this );

		if (foundIt != m_Parent->m_Children.end( ))
			m_Parent->m_Children.erase( foundIt );

		m_Parent = nullptr;
	}

	m_Parent = target;

	// Add as child
	if (m_Parent)
	{
		m_Parent->m_Children.push_back( this );
	}
}

void CActor::SpawnActor( const string& name, CActor* parent/* = nullptr*/ )
{
	CActor* newActor = new CActor( m_Scene );
	newActor->SetName( name );
	newActor->SetParent( parent );

	newActor->Init( );
}
