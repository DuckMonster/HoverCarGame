#pragma once
#include <set>
#include <unordered_map>
#include <net/P2PServer.h>
#include <net/P2PClient.h>
#include <net/Messaging.h>
#include "NetworkTypes.h"
#include "DataStructure.h"

class CGame;

namespace Network
{
	class CActorChannel;

	extern const bool _IsServer;
	_forceinline bool IsServer()
	{
		return _IsServer;
	}

	class CNetwork
	{
	public:
		typedef std::set<neteng::peer_hash>::iterator peer_iterator;

	public:
		CNetwork();
		bool StartUp( CGame* game, const CDataStructure& data );
		void PollEvents();

		void SendPeer( const neteng::Packet& packet, neteng::PacketType type, neteng::peer_hash peer );
#if !defined(GAME_SERVER)
		void SendServer( const neteng::Packet& packet, neteng::PacketType type );
#endif

		void RegisterChannel( CActorChannel* channel );
		void UnregisterChannel( CActorChannel* channel );
		CActorChannel* GetChannel( const channel_id id ) { return m_ChannelMap[id]; }
		CActorChannel* GetOrCreateChannel( const channel_id id );

		neteng::peer_hash GetLocalHash();
		peer_iterator PeerBegin() { return m_PeerSet.begin(); }
		peer_iterator PeerEnd() { return m_PeerSet.end(); }

		CGame* GetGame() const { return m_Game; }

	private:

		void HandleMessage( neteng::Packet packet );

		CGame* m_Game;

		std::set<neteng::peer_hash> m_PeerSet;

		std::vector<CActorChannel*> m_ChannelList;
		std::unordered_map<channel_id, CActorChannel*> m_ChannelMap;

#if defined(GAME_SERVER)
		neteng::P2PServer m_Server;
#else
		neteng::P2PClient m_Client;
#endif
	};

}