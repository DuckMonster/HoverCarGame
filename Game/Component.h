#pragma once
#include <net/Packet.h>
#include <stack>
#include "ActorData.h"
#include "DataStructure.h"
#include "ComponentFactory.h"
#include "Serialize.h"
#include "RPC.h"
#include "Pointers.h"

namespace Network
{
	class CActorChannel;
}

class CActor;
class CTransform;
class CScene;
class CGame;

class CComponent : public CPtrType
{
	typedef void( CComponent::*Comp_RPC_Func )( const neteng::Packet& packet );

public:
	CComponent( class CActor* actor );
	CComponent( const CComponent& other );
	virtual ~CComponent();

	CComponent* Copy( CActor* actor ) const;

	//--------------------------------------------------- Events
	virtual void OnInit();
	virtual void OnLoad( const CDataStructure& data );
	virtual void OnBegin();
	virtual void OnDestroy();

	void UpdateInternal( const SUpdateInfo& info );
	virtual void OnPostUpdate( const SUpdateInfo& info ) {}
	virtual void OnUpdate( const SUpdateInfo& info ) {}
	virtual void OnPreUpdate( const SUpdateInfo& info ) {}

	virtual void OnTransformCleaned() {}

#if !defined(GAME_SERVER)
	virtual void OnRender( const SRenderInfo& info ) {}
#endif
	//---------------------------------------------------

	//--------------------------------------------------- Typing
	ComponentType GetType() const { return m_TypeStack.empty() ? ComponentType() : m_TypeStack[m_TypeStack.size() - 1]; }
	template<typename T>
	bool IsA() const
	{
		return IsA( CComponentFactory::GetType<T>() );
	}
	bool IsA( ComponentType type )
	{
		for ( size_t i = 0; i < m_TypeStack.size(); i++ )
		{
			if ( m_TypeStack[i] == type )
				return true;
		}

		return false;
	}

	// Actor
	void SetActor( CActor* actor ) { EnsureMsg( !m_Actor, "Already has an owner" ); m_Actor = actor; }

	//--------------------------------------------------- Getters
	CActor* GetActor() const { return m_Actor; }
	CTransform* Transform() const;
	CScene* GetScene() const;
	CGame* GetGame() const;
	Network::CActorChannel* GetChannel();

	//--------------------------------------------------- Name
	const char* GetName() const { return GetType().name; }
	virtual std::string GetDisplayString() const { return std::string( "{}" ); }


	//--------------------------------------------------- Networking
	virtual void Serialize( CSerializer& serializer );
	void ReceiveRPC( const Network::RPC_Source& rpc, const neteng::Packet& packet );

protected:
	//--------------------------------------------------- Register Type
	template<class T>
	void Register()
	{
		ComponentType type = CComponentFactory::GetType<T>();

		Ensure( !IsA( type ) );
		m_TypeStack.push_back( type );
	}

	//--------------------------------------------------- Networking
	template<class FuncT>
	Network::RPC_Handle* RegisterRPC( FuncT func, const Network::RPC_Policy& policy )
	{
		rpc_id id = static_cast<rpc_id>( m_RPCList.size() );
		RPC_Handle* handle = CreateRPC( func, this, id, policy );
		m_RPCList.emplace_back( handle );

		return handle;
	}

private:
	class CActor* m_Actor;
	bool m_HasBegun = false;

	//--------------------------------------------------- Typing
	std::vector<ComponentType> m_TypeStack;

	//--------------------------------------------------- Networking
	std::vector<Network::RPC_Handle*> m_RPCList;
};