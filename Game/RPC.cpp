#include "stdafx.h"
#include "RPC.h"
#include "Actor.h"
#include "Component.h"
#include "ActorChannel.h"
#include "Game.h"

namespace Network
{
	/**	Constructor
	*******************************************************************************/
	RPC_Handle::RPC_Handle( CComponent* component, rpc_id id, const RPC_Policy& defaultPolicy ) :
		m_Component( component ), m_Id( id ), m_DefaultPolicy( defaultPolicy )
	{
	}

	/**	Send
	*******************************************************************************/
	void RPC_Handle::Send_Internal( neteng::Packet & packet, const RPC_Policy& policy )
	{
		if ( PolicyIncludesSelf( policy ) )
			Recv( packet );

		RPC_Source source;
		source.component = m_Component->GetType().id;
		source.rpc = m_Id;
		m_Component->GetChannel()->SendRPC( source, packet, policy );
	}

	/**	Policy Includes Self
	*******************************************************************************/
	bool RPC_Handle::PolicyIncludesSelf( const RPC_Policy& policy )
	{
		switch ( policy.target )
		{
		case RPC_Target::BroadcastAll:
			return true;

		case RPC_Target::BroadcastPeers:
#if defined(GAME_SERVER)
			return false;
#else
			return true;
#endif

		case RPC_Target::Peers:
#if defined(GAME_SERVER)
			return false;
#else
			{
				peer_hash localHash = m_Component->GetGame()->GetNetwork()->GetLocalHash();
				return ( std::find( policy.peers, policy.peers + policy.peer_num, localHash )
					== policy.peers + policy.peer_num );
			}
#endif

		case RPC_Target::Server:
#if defined(GAME_SERVER)
			return true;
#else
			return false;
#endif
		}

		return false;
	}
}
