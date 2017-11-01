#pragma once
#include "Input.h"
#include "ActorData.h"
#include "Network.h"
#include "Serialize.h"
#include "RPC.h"
#include "Pointers.h"
#include "Name.h"

class CComponent;

namespace Network
{
	class CActorChannel;
}

class CActor : public CPtrType
{
private:
	typedef std::vector<CActor*>::iterator actor_iterator;
	typedef std::vector<CComponent*>::iterator component_iterator;

	struct DestroyDeferrer
	{
	public:
		DestroyDeferrer( CActor* actor ) : actor( actor ) { actor->PushDestroyDefer(); }
		~DestroyDeferrer() { actor->PopDestroyDefer(); }

	private:
		CActor* actor;
	};

public:
	CActor( class CScene* scene );
	CActor( const CActor& other );

	void Init();
	void Destroy();

	void Update( const SUpdateInfo& info );
	void OnTransformCleaned();

#if !defined(GAME_SERVER)
	void Render( const SRenderInfo& info );
#endif

	// Tree helping functions
	size_t NumChildren() const;
	CActor* GetParent() const;
	CActor* GetChild( size_t index ) const;

	CActor* Find( const CName& name );
	CActor* FindChild( const CName& name );
	CActor* EvaluatePath( const CPath& path, const size_t index = 0 );

	// Spawning
	CActor* SpawnActor( const std::string& name = "", CActor* parent = nullptr );
	CActor* Clone( CActor* parent = nullptr );

	void SetName( const CName& name ) { m_Name = name; SetPathDirty(); }
	const CName& GetName() const { return m_Name; }

	void SetVisible( bool visible ) { m_Visible = visible; }
	bool GetVisible() const { return m_Visible; }

	const CPath& GetPath();
	void SetPathDirty();

	// Components
	template<typename TComp>
	TComp* AddComponent();
	CComponent* AddComponent( CComponent* component );
	CComponent* AddComponentCopy( CComponent* other );

	template<typename TComp>
	TComp* GetComponent();
	CComponent* GetComponent( component_type_id id ) const;
	CComponent* GetComponentAt( int index ) const { return m_Components[index]; }

	template<typename TComp>
	TComp* FindComponent();

	template<typename TComp>
	bool RemoveComponent();
	bool RemoveComponent( CComponent* component );

	size_t NumComponents() const { return m_Components.size(); }

	void ReLoadComponents() const;

	template<typename TComp>
	void CollectComponents( std::vector<TComp*>& list );

	// Scripts
	void AttachScript( ActorScript script ) { m_Scripts.push_back( script ); }
	size_t NumScripts() { return m_Scripts.size(); }

	// Some getters
	class CTransform* Transform() const { return m_Transform; }
	class CScene* Scene() const { return m_Scene; }
	class CGame* Game() const { return m_Game; }

	// Network TEMP
	void Replicate( Network::channel_id id );
	void Replicate( Network::CActorChannel* channel );
	bool IsReplicated() const { return m_Channel != nullptr; }
	void ReplicateTo( neteng::peer_hash peer );
	void ReceiveRPC( const Network::RPC_Source& rpc, const neteng::Packet& packet );

	Network::CActorChannel* GetChannel() const { return m_Channel; }

	void Serialize( CSerializer& Serializer );

private:
	// Destroy deferring
	void Destroy_Internal();
	void PushDestroyDefer();
	void PopDestroyDefer();
	int m_DeferDestroyStack = 0;

private:
	class CScene* const	m_Scene;
	class CGame* const	m_Game;

	// Transform
	class CTransform* m_Transform;
	CPath m_PathCache;
	bool m_PathDirty = true;

	// Components
	std::vector<CComponent*> m_Components;

	// Networking
	Network::CActorChannel* m_Channel = nullptr;

	// Scripts
	std::vector<ActorScript> m_Scripts;

	CName m_Name = "";
	bool m_Visible = true;

	bool m_ShouldDestroy = false;
	bool m_IsDestroying = false;
};

//--------------------------------------------------- Inlined Functions
/**	AddComponent
*******************************************************************************/
template<typename TComp>
inline TComp* CActor::AddComponent()
{
	TComp* newComponent = new TComp( this );
	AddComponent( newComponent );
	return newComponent;
}

/**	Get Component
*******************************************************************************/
template<typename TComp>
inline TComp* CActor::GetComponent()
{
	ComponentType type = CComponentFactory::GetType<TComp>();
	return (TComp*)GetComponent( type.id );
}

/**	Find Component
*******************************************************************************/
template<typename TComp>
inline TComp* CActor::FindComponent()
{
	TComp* component = GetComponent<TComp>();
	if ( component )
		return component;

	// Search children
	for ( size_t i = 0; i < NumChildren(); i++ )
	{
		component = GetChild( i )->FindComponent<TComp>();
		if ( component )
			return component;
	}

	// No component found
	return nullptr;
}

/**	Remove Component
*******************************************************************************/
template<typename TComp>
inline bool CActor::RemoveComponent()
{
	// Type-check cast
	TComp* comp = GetComponent<TComp>();

	if ( comp )
	{
		comp->OnDestroy();
		delete comp;

		m_Components.erase( m_Components.begin() + i );
		return true;
	}

	return false;
}

/**	Collect Component
*******************************************************************************/
template<typename TComp>
inline void CActor::CollectComponents( std::vector<TComp*>& list )
{
	TComp* comp = GetComponent<TComp>();
	if ( comp )
		list.push_back( comp );

	for ( size_t i = 0; i < NumChildren(); i++ )
		GetChild( i )->CollectComponents<TComp>( list );
}
