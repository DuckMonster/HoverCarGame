#pragma once
#include <net/Packet.h>
#include <net/Hash.h> 
#include <queue>
#include "NetworkTypes.h"
#include "RPC.h"

class CActor;

namespace Network
{
	class CNetwork;

	class CActorChannel
	{
		struct RPCQueueData
		{
			RPC_Source source;
			neteng::Packet packet;
		};

	public:
		CActorChannel( CNetwork* network );
		CActorChannel( CNetwork* network, channel_id id );
		CActorChannel( CNetwork* network, channel_id id, CActor* actor );
		~CActorChannel();

		void Open( Network::channel_id id );
		void Open( Network::channel_id id, CActor* actor );
		void Close();

		void RecvNetActor( const neteng::Packet& packet );
		void SendRPC( RPC_Source& rpc, neteng::Packet& packet, const RPC_Policy& policy );
		void RecvRPC( const RPC_Source& rpc, const neteng::Packet& packet );

		void AddPeer( peer_hash peer );
		void AddPeers( peer_hash* peers, size_t count );
		void RemovePeer( peer_hash peer );
		void RemovePeers( peer_hash* peers, size_t count );

		bool IsValid() const { return m_ChannelId != 0; }
		channel_id GetId() const { return m_ChannelId; }
		CActor* GetActor() const { return m_Actor; }

	private:
		CNetwork* const m_Network;

		CActor* m_Actor = nullptr;
		Network::channel_id	m_ChannelId = 0;

		std::set<peer_hash> m_Peers;
		std::queue<RPCQueueData> m_RPCQueue;
	};
}