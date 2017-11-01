#pragma once
#include "ComponentFactory.h"
#include "NetworkTypes.h"
#include "Serialize.h"
#include <net/Packet.h>

class CComponent;

namespace Network
{
	struct RPC_Source
	{
		channel_id channel;
		component_type_id component;
		rpc_id rpc;
	};

	enum class RPC_Target
	{
		BroadcastAll,
		BroadcastPeers,
		Server,
		Peers
	};

	struct RPC_Policy
	{
		static RPC_Policy BroadcastAll() { return { RPC_Target::BroadcastAll, nullptr, 0 }; }
		static RPC_Policy BroadcastPeers() { return { RPC_Target::BroadcastPeers, nullptr, 0 }; }
		static RPC_Policy Server() { return { RPC_Target::Server, nullptr, 0 }; }
		static RPC_Policy Peers( neteng::peer_hash* peers, size_t peer_num ) { return { RPC_Target::Peers, peers, peer_num }; }
		static RPC_Policy Peer( const neteng::peer_hash& peer ) { return { RPC_Target::Peers, &peer, 1 }; }

		RPC_Target target;
		const neteng::peer_hash* peers;
		size_t peer_num;
	};

	struct RPC_Handle
	{
	public:
		template<typename... TArgs>
		void SendPolicy( const RPC_Policy& policy, TArgs... args )
		{
			CSerializer serialize( m_Component->GetScene() );
			ArgsToMem( serialize, args... );

			Send_Internal( neteng::Packet( serialize.Data(), serialize.Size() ), policy );
		}
		template<typename... TArgs>
		void operator()( TArgs... args ) { Send( args... ); }
		template<typename... TArgs>
		void Send( TArgs... args ) { SendPolicy( m_DefaultPolicy, args... ); }
		void Recv( const neteng::Packet& packet ) { Recv_Internal( packet ); }

	protected:
		RPC_Handle( CComponent* component, rpc_id id, const RPC_Policy& defaultPolicy );

		//--------------------------------------------------- Args To Mem
		void ArgsToMem( CSerializer& s ) { }

		template<typename T, typename... TArgs>
		void ArgsToMem( CSerializer& s, T& value, TArgs... rest )
		{
			s.Serialize( value );
			ArgsToMem( s, rest... );
		}
		//---------------------------------------------------

		//--------------------------------------------------- Final Functions
		void Send_Internal( neteng::Packet& packet, const RPC_Policy& policy );
		virtual void Recv_Internal( const neteng::Packet& packet ) = 0;
		//---------------------------------------------------

		bool PolicyIncludesSelf( const RPC_Policy& policy );

		rpc_id m_Id;
		CComponent* m_Component;
		RPC_Policy	m_DefaultPolicy;
	};

	template<typename... TArgs>
	struct RPC : public RPC_Handle
	{
		typedef void( CComponent::*RPC_Func )( TArgs... );
		RPC( RPC_Func func, CComponent* component, rpc_id id, const RPC_Policy& policy ) :
			RPC_Handle( component, id, policy ), m_Func( func ) {}

	private:

		void Recv_Internal( const neteng::Packet& packet ) override
		{
			MemToArgs<TArgs...>( CSerializer( packet.Data(), packet.Size(), m_Component->GetScene() ) );
		}

		//--------------------------------------------------- Vars
		RPC_Func m_Func;

		//--------------------------------------------------- Mem To Args
		template<typename... TResult>
		void MemToArgs( CSerializer& s, TResult... result )
		{
			( ( *m_Component ).*m_Func )( result... );
		}

		template<typename T, typename... TRest, typename... TResult>
		void MemToArgs( CSerializer& s, TResult... result )
		{
			std::remove_reference<T>::type value;
			s.Serialize( value );

			MemToArgs<TRest...>( s, result..., value );
		}
		//---------------------------------------------------
	};

	template<typename T, typename ...TArgs>
	inline RPC_Handle* CreateRPC( void( T::*func )( TArgs... ), CComponent* component, rpc_id id, const RPC_Policy& policy )
	{
		typedef void ( CComponent::*Comp_Base_Func )( TArgs... );
		return new RPC<TArgs...>( static_cast<Comp_Base_Func>( func ), component, id, policy );
	}
}