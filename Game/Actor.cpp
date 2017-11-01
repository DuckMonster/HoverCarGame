#include "stdafx.h"
#include "Actor.h"
#include "Scene.h"
#include "Component.h"
#include "Transform.h"
#include "Color.h"
#include "Gizmo.h"
#include "StringUtils.h"
#include "ActorChannel.h"

#include "Game.h"

using namespace std;
using namespace Network;

/**	Constructor
*******************************************************************************/
CActor::CActor( CScene* scene ) :
	m_Scene( scene ),
	m_Game( scene->GetGame() )
{
	m_Transform = new CTransform( this );
}

/**	Copy Constructor
*******************************************************************************/
CActor::CActor( const CActor& other ) : CActor( other.m_Scene )
{
	SetName( other.GetName() );
	Transform()->SetParent( nullptr );

	for ( size_t i = 0; i < other.NumComponents(); i++ )
		AddComponentCopy( other.m_Components[i] );

	for ( const ActorScript& script : other.m_Scripts )
		m_Scripts.emplace_back( script );
}

/**	Init
*******************************************************************************/
void CActor::Init()
{
}

/**	Destroy
*******************************************************************************/
void CActor::Destroy()
{
	if ( m_ShouldDestroy || m_IsDestroying )
		return;

	if ( m_DeferDestroyStack > 0 )
		m_ShouldDestroy = true;
	else
		Destroy_Internal();
}

/**	Update
*******************************************************************************/
void CActor::Update( const SUpdateInfo& data )
{
	DestroyDeferrer defer( this );

	// Scripts
	if ( data.Phase == UpdatePhase::Main )
		for ( size_t i = 0; i < m_Scripts.size(); i++ )
			m_Scripts[i]( this, data );

	// Components
	for ( size_t i = 0; i < m_Components.size(); i++ )
		m_Components[i]->UpdateInternal( data );

	// Children
	for ( size_t i = 0; i < NumChildren(); i++ )
		GetChild( i )->Update( data );
}

/**	On Transform Cleaned
*******************************************************************************/
void CActor::OnTransformCleaned()
{
	DestroyDeferrer defer( this );

	// Components
	for ( size_t i = 0; i < m_Components.size(); i++ )
		m_Components[i]->OnTransformCleaned();
}

#if !defined(GAME_SERVER)
/**	Render
*******************************************************************************/
void CActor::Render( const SRenderInfo& info )
{
	DestroyDeferrer defer( this );

	if ( !m_Visible )
		return;

	for ( size_t i = 0; i < m_Components.size(); i++ )
		m_Components[i]->OnRender( info );

	for ( size_t i = 0; i < NumChildren(); i++ )
		GetChild( i )->Render( info );

	//--------------------------------------------------- Debug Draw
	if ( Scene()->GetGame()->GetDebugDraw() )
	{
		glm::vec3 position = Transform()->GetWorldPosition(),
			forward = Transform()->GetForward(),
			right = Transform()->GetRight(),
			up = Transform()->GetUp();

		CGizmo::DrawLine( position, position + forward, CColor::Red, 2.f, info.CameraMatrix );
		CGizmo::DrawLine( position, position + up, CColor::Green, 2.f, info.CameraMatrix );
		CGizmo::DrawLine( position, position + right, CColor::Blue, 2.f, info.CameraMatrix );

		for ( size_t i = 0; i < NumChildren(); i++ )
		{
			CGizmo::DrawLine( position, GetChild( i )->Transform()->GetWorldPosition(),
				CColor::White, 1.f,
				info.CameraMatrix
			);
		}
	}
}
#endif

/**	Num Children
*******************************************************************************/
size_t CActor::NumChildren() const
{
	return m_Transform->NumChildren();
}

/**	Get Parent
*******************************************************************************/
CActor* CActor::GetParent() const
{
	return m_Transform->GetParent() ? m_Transform->GetParent()->GetActor() : nullptr;
}

/**	Get Child
*******************************************************************************/
CActor* CActor::GetChild( size_t index ) const
{
	return m_Transform->GetChild( index )->GetActor();
}

/**	Find
*******************************************************************************/
CActor* CActor::Find( const CName& name )
{
	CActor* result = FindChild( name );

	// This is my child
	if ( result )
		return result;

	// Search in children...
	for ( size_t i = 0; i < NumChildren(); i++ )
	{
		result = GetChild( i )->Find( name );
		if ( result )
			return result;
	}

	return nullptr;
}

/**	Find Child
*******************************************************************************/
CActor* CActor::FindChild( const CName& name )
{
	//--------------------------------------------------- When in DEBUG, double check that duplicate names don't exist
#if defined(_DEBUG)
	CActor* child = nullptr;

	for ( size_t i = 0; i < NumChildren(); i++ )
	{
		CActor* c = GetChild( i );

		if ( c->GetName() == name )
		{
			EnsureMsg( child == nullptr, "Found duplicate child with name %s", name.c_str() );
			child = c;
		}
	}

	return child;
#else
	for ( size_t i = 0; i < NumChildren(); i++ )
	{
		if ( GetChild( i )->GetName() == name )
			return GetChild( i );
	}

	return nullptr;
#endif
}

/**	Evaluate Path
*******************************************************************************/
CActor* CActor::EvaluatePath( const CPath& path, const size_t index/* = 0*/ )
{
	CActor* result = FindChild( path[index] );

	if ( !result )
		return nullptr;

	// End of path!
	if ( index == path.Length() - 1 )
		return result;

	// ... or continue down it
	else
		return result->EvaluatePath( path, index + 1 );
}

/**	Spawn Actor
*******************************************************************************/
CActor* CActor::SpawnActor( const string& name, CActor* parent/* = nullptr*/ )
{
	if ( parent == nullptr )
		parent = this;

	CActor* newActor = new CActor( m_Scene );
	newActor->SetName( name.length() > 0 ? name : parent->GetName() );
	if ( parent )
		newActor->m_Transform->SetParent( parent->m_Transform );

	newActor->Init();

	return newActor;
}

/**	Clone
*******************************************************************************/
CActor* CActor::Clone( CActor* parent/* = nullptr*/ )
{
	if ( parent == nullptr )
		parent = GetParent();

	CActor* newActor = new CActor( *this );
	newActor->m_Transform->SetParent( parent ? parent->m_Transform : nullptr );

	newActor->Init();

	return newActor;
}

/**	Get Path
*******************************************************************************/
const CPath& CActor::GetPath()
{
	if ( !m_PathDirty )
		return m_PathCache;

	if ( GetParent() )
	{
		m_PathCache = GetParent()->GetPath() + GetName();
	}
	else
	{
		m_PathCache = "";
	}

	m_PathDirty = false;
	return m_PathCache;
}

/**	Set Path Dirty
*******************************************************************************/
void CActor::SetPathDirty()
{
	m_PathDirty = true;

	for ( size_t i = 0; i < NumChildren(); i++ )
		GetChild( i )->SetPathDirty();
}

/**	Add Component
*******************************************************************************/
CComponent* CActor::AddComponent( CComponent* component )
{
	m_Components.push_back( component );

	EnsureMsg( component->GetType().IsValid(), "Component %s wasn't registered!", component->GetName() );

	component->OnInit();
	component->OnLoad( Scene()->GetInitData() );

	return component;
}

/**	Add Component Copy
*******************************************************************************/
CComponent* CActor::AddComponentCopy( CComponent * other )
{
	CComponent* copy = other->Copy( this );
	AddComponent( copy );

	return copy;
}

/**	Get Component
*******************************************************************************/
CComponent* CActor::GetComponent( component_type_id id ) const
{
	ComponentType type;
	type.id = id;

	for ( size_t i = 0; i < NumComponents(); i++ )
	{
		if ( m_Components[i]->IsA( type ) )
			return m_Components[i];
	}

	return nullptr;
}

/**	Remove Component
*******************************************************************************/
bool CActor::RemoveComponent( CComponent* component )
{
	// Find specific component
	vector<CComponent*>::iterator it = find( m_Components.begin(), m_Components.end(), component );
	if ( it == m_Components.end() )
		return false;

	// Call destroy event
	component->OnDestroy();
	delete component;

	m_Components.erase( it );
	return true;
}

/**	ReInitialize Components
*******************************************************************************/
void CActor::ReLoadComponents() const
{
	for ( size_t i = 0; i < NumComponents(); i++ )
	{
		m_Components[i]->OnLoad( Scene()->GetInitData() );
	}

	for ( size_t i = 0; i < NumChildren(); i++ )
	{
		GetChild( i )->ReLoadComponents();
	}
}

/**	Replicate
*******************************************************************************/
void CActor::Replicate( Network::channel_id id )
{
	if ( EnsureNot( IsReplicated() ) )
		return;

	m_Channel = new CActorChannel( Game()->GetNetwork(), id, this );
}

/**	Replicate
*******************************************************************************/
void CActor::Replicate( Network::CActorChannel* channel )
{
	if ( EnsureNot( IsReplicated() ) )
		return;

	m_Channel = channel;
}

/**	Replicate To
*******************************************************************************/
void CActor::ReplicateTo( peer_hash peer )
{
	if ( EnsureNot( !IsReplicated() ) )
		return;

	m_Channel->AddPeer( peer );
}

/**	Receive RPC
*******************************************************************************/
void CActor::ReceiveRPC( const Network::RPC_Source& rpc, const neteng::Packet& packet )
{
	CComponent* comp = GetComponent( rpc.component );

	if ( EnsureNotMsg( !comp, "Received RPC for unknown component %d", rpc.component ) )
		return;

	comp->ReceiveRPC( rpc, packet );
}

/**	Serialize
*******************************************************************************/
void CActor::Serialize( CSerializer& Serializer )
{
	Serializer.Serialize( m_Name );
	m_Transform->Serialize( Serializer );

	if ( Serializer.IsWriting() )
	{
		Serializer.Serialize( NumComponents() );
		for ( CComponent* comp : m_Components )
		{
			Serializer.Serialize( comp->GetType().id );
			comp->Serialize( Serializer );
		}
	}
	else
	{
		size_t numComps;
		Serializer.Serialize( numComps );

		for ( size_t i = 0; i < numComps; i++ )
		{
			component_type_id id;
			Serializer.Serialize( id );

			// Make new component
			CComponent* comp = CComponentFactory::GetType( id ).funcs.make( this );
			AddComponent( comp );
			comp->Serialize( Serializer );
		}
	}
}

/**	Destroy_Internal
*******************************************************************************/
void CActor::Destroy_Internal()
{
	if ( m_IsDestroying )
		return;

	m_IsDestroying = true;

	for ( size_t i = 0; i < NumChildren(); i++ )
	{
		CActor* child = GetChild( i );
		child->Destroy();
	}

	while ( !m_Components.empty() )
	{
		CComponent* comp = m_Components[0];
		m_Components.erase( m_Components.begin() );

		comp->OnDestroy();
		delete comp;
	}

	m_Transform->SetParent( nullptr );
	delete m_Transform;
	m_Transform = nullptr;

	if ( m_Channel )
		delete m_Channel;

	m_Channel = nullptr;
	delete this;
}

/**	Push Destroy Defer
*******************************************************************************/
void CActor::PushDestroyDefer()
{
	m_DeferDestroyStack++;
}

/**	Pop Destroy Defer
*******************************************************************************/
void CActor::PopDestroyDefer()
{
	m_DeferDestroyStack--;

	if ( m_ShouldDestroy )
		Destroy_Internal();
}
