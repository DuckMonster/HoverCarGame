#include "stdafx.h"
#include "Network.h"
#include "Game.h"
#include "Scene.h"
#include "ActorChannel.h"
#include "RPC.h"

using namespace neteng;

namespace Network
{
#if defined(GAME_SERVER)
	const bool _IsServer = true;
#else
	const bool _IsServer = false;
#endif

	/**	Constructor
	*******************************************************************************/
	CNetwork::CNetwork()
	{
		neteng::NetLogger::SetEnabled( false );
	}

	/**	Start Up
	*******************************************************************************/
	bool CNetwork::StartUp( CGame * game, const CDataStructure & data )
	{
		m_Game = game;

#if defined(GAME_SERVER)
		int port = 5555;
		data.Read( "/Port", port );

		m_Server.StartUp( port );
		return m_Server.IsActive();
#else
		std::string hostname = "localhost";
		int port = 5555;
		data.Read( "/Port", port );
		data.Read( "/HostName", hostname );

		m_Client.Connect( hostname.c_str(), port );
		return m_Client.IsConnected();
#endif
	}

	/**	Poll Events
	*******************************************************************************/
	void CNetwork::PollEvents()
	{
		NetEvent event;

#if defined(GAME_SERVER)
		while ( m_Server.PollEvent( event ) )
		{
			switch ( event.type )
			{
			case NetEventType::PeerConnected:
				for ( peer_hash peer : m_PeerSet )
					m_Server.OpenChannel( peer, event.peer );

				m_PeerSet.emplace( event.peer );
				break;

			case NetEventType::PeerDisconnected:
				m_PeerSet.erase( event.peer );

				for ( CActorChannel* channel : m_ChannelList )
					channel->RemovePeer( event.peer );
				break;

			case NetEventType::Message:
				HandleMessage( event.packet );
				break;
			}

			m_Game->OnNetEvent( event );
		}
#else
		while ( m_Client.PollEvent( event ) )
		{
			if ( event.type == NetEventType::ChannelOpened )
			{
				m_PeerSet.emplace( event.peer );
			}
			else if ( event.type == NetEventType::Message )
				HandleMessage( event.packet );

			m_Game->OnNetEvent( event );
		}
#endif
	}

	/**	Send
	*******************************************************************************/
	void CNetwork::SendPeer( const Packet& packet, PacketType type, peer_hash peer )
	{
#if defined(GAME_SERVER)
		m_Server.Send( packet, type, peer );
#else
		m_Client.Send( packet, type, NetActor::Peer, peer );
#endif
	}

#if !defined(GAME_SERVER)
	void CNetwork::SendServer( const Packet& packet, PacketType type )
	{
		m_Client.Send( packet, type, NetActor::Server );
	}
#endif

	/**	Register Channel
	*******************************************************************************/
	void CNetwork::RegisterChannel( CActorChannel* channel )
	{
		if ( !Ensure( channel->IsValid() ) )
			return;

		if ( !Ensure( m_ChannelMap[channel->GetId()] == nullptr ) )
			return;

		m_ChannelMap[channel->GetId()] = channel;
		m_ChannelList.emplace_back( channel );
		Debug_Log( "ActorChannel [%d] registered", channel->GetId() );
	}

	/**	Unregister Channel
	*******************************************************************************/
	void CNetwork::UnregisterChannel( CActorChannel* channel )
	{
		if ( EnsureNot( !channel->IsValid() ) )
			return;

		channel_id id = channel->GetId();
		m_ChannelMap.erase( id );

		std::vector<CActorChannel*>::iterator it = find( m_ChannelList.begin(), m_ChannelList.end(), channel );
		if ( it != m_ChannelList.end() )
			m_ChannelList.erase( it );
	}

	/**	Get Or Create Channel
	*******************************************************************************/
	CActorChannel* CNetwork::GetOrCreateChannel( const channel_id id )
	{
		CActorChannel* channel = m_ChannelMap[id];

		if ( !channel )
			channel = new CActorChannel( this, id );

		return channel;
	}

	/**	Get Local Hash
	*******************************************************************************/
	peer_hash CNetwork::GetLocalHash()
	{
#if defined(GAME_SERVER)
		return 0;
#else
		return m_Client.GetLocalHash();
#endif
	}

	/**	Handle Message
	*******************************************************************************/
	void CNetwork::HandleMessage( neteng::Packet packet )
	{
		GameMessage message = packet.PopHeader<GameMessage>();

		switch ( message )
		{
		case GameMessage::Scene:
			m_Game->GetScene()->OnNetMessage( packet );
			break;

		case GameMessage::Channel_Close:
		{
			channel_id id = packet.PopHeader<channel_id>();
			CActorChannel* channel = GetChannel( id );

			if ( EnsureNot( !channel ) )
				break;

			channel->Close();
		} break;

		case GameMessage::Channel_Actor:
		{
			channel_id id = packet.PopHeader<channel_id>();
			CActorChannel* channel = GetOrCreateChannel( id );

			channel->RecvNetActor( packet );
		} break;

		case GameMessage::Channel_RPC:
		{
			const RPC_Source& rpc = packet.PopHeader<RPC_Source>();
			CActorChannel* channel = GetOrCreateChannel( rpc.channel );

			channel->RecvRPC( rpc, packet );
		} break;

		case GameMessage::Channel_AddPeers:
		{
			channel_id id = packet.PopHeader<channel_id>();
			size_t count = packet.PopHeader<unsigned char>();
			peer_hash* peer = (peer_hash*)packet.Data();

			CActorChannel* channel = GetOrCreateChannel( id );
			channel->AddPeers( peer, count );
		} break;

		case GameMessage::Channel_RemovePeers:
		{
			channel_id id = packet.PopHeader<channel_id>();
			size_t count = packet.PopHeader<unsigned char>();
			peer_hash* peer = (peer_hash*)packet.Data();

			CActorChannel* channel = GetOrCreateChannel( id );
			channel->RemovePeers( peer, count );
		} break;
		}
	}
}