#include "stdafx.h"
#include "ActorChannel.h"
#include "Network.h"
#include "Serialize.h"
#include "Actor.h"
#include "Game.h"
#include "Scene.h"
#include "StringUtils.h"

using namespace neteng;

namespace Network
{
	/**	Constructor
	*******************************************************************************/
	CActorChannel::CActorChannel( CNetwork * network ) : m_Network( network )
	{
	}

	/**	Constructor
	*******************************************************************************/
	CActorChannel::CActorChannel( CNetwork * network, channel_id id ) : CActorChannel( network )
	{
		Open( id );
	}

	/**	Constructor
	*******************************************************************************/
	CActorChannel::CActorChannel( CNetwork * network, channel_id id, CActor * actor ) : CActorChannel( network )
	{
		Open( id, actor );
	}

	/** Destructor
	*******************************************************************************/
	CActorChannel::~CActorChannel()
	{
		Close();
	}

	/**	Open
	*******************************************************************************/
	void CActorChannel::Open( Network::channel_id id )
	{
		if ( EnsureNot( IsValid() ) )
			return;

		Debug_Log( "Actor Channel [%d] opened", id );

		m_ChannelId = id;
		m_Network->RegisterChannel( this );
	}

	/**	Open
	*******************************************************************************/
	void CActorChannel::Open( Network::channel_id id, CActor* actor )
	{
		if ( EnsureNot( IsValid() ) )
			return;

		Debug_Log( "Actor Channel [%d] opened", id );

		m_ChannelId = id;
		m_Actor = actor;
		m_Network->RegisterChannel( this );
	}

	/**	Close
	*******************************************************************************/
	void CActorChannel::Close()
	{
		if ( !IsValid() )
			return;

#if defined(GAME_SERVER)
		neteng::Packet packet( nullptr, 0 );
		packet.PushHeader( m_ChannelId );
		packet.PushHeader( GameMessage::Channel_Close );

		for ( peer_hash peer : m_Peers )
			m_Network->SendPeer( packet, PacketType::Reliable, peer );
#endif

		m_Network->UnregisterChannel( this );

		m_ChannelId = 0;
		CActor* actor = m_Actor;
		m_Actor = nullptr;

		if ( actor )
			actor->Destroy();
	}

	/**	Receive Net Actor
	*******************************************************************************/
	void CActorChannel::RecvNetActor( const neteng::Packet & packet )
	{
		if ( EnsureNot( m_Actor ) )
			return;

		Debug_Log( "Received actor for Channel [%d]", m_ChannelId );

		CScene* scene = m_Network->GetGame()->GetScene();
		CActor* actor = scene->SpawnActor( StringUtils::Printf( "NetActor%d", m_ChannelId ) );

		// Deserialize actor
		CSerializer serializer( packet.Data(), packet.Size(), scene );
		actor->Serialize( serializer );
		actor->Replicate( this );

		m_Actor = actor;

		// Trigger queued RPC's
		while ( !m_RPCQueue.empty() )
		{
			RPCQueueData& data = m_RPCQueue.front();
			RecvRPC( data.source, data.packet );

			m_RPCQueue.pop();
		}
	}

	/**	Send RPC
	*******************************************************************************/
	void CActorChannel::SendRPC( RPC_Source& rpc, Packet& packet, const RPC_Policy& policy )
	{
		rpc.channel = m_ChannelId;
		packet.PushHeader( rpc );
		packet.PushHeader( GameMessage::Channel_RPC );

		switch ( policy.target )
		{
		case RPC_Target::BroadcastAll:
			for ( peer_hash peer : m_Peers )
				m_Network->SendPeer( packet, PacketType::Unreliable, peer );
#if !defined(GAME_SERVER)
			m_Network->SendServer( packet, PacketType::Unreliable );
#endif
			break;

		case RPC_Target::BroadcastPeers:
			for ( peer_hash peer : m_Peers )
				m_Network->SendPeer( packet, PacketType::Unreliable, peer );
			break;

#if !defined(GAME_SERVER)
		case RPC_Target::Server:
			for ( peer_hash peer : m_Peers )
				m_Network->SendPeer( packet, PacketType::Unreliable, peer );
			break;
#endif

		case RPC_Target::Peers:
			for ( size_t i = 0; i < policy.peer_num; i++ )
				m_Network->SendPeer( packet, PacketType::Unreliable, policy.peers[i] );
			break;
		}

		//Debug_Log( "Sending RPC ( %d, %d, %d )", rpc.channel, rpc.component, rpc.rpc );
	}

	/**	Receive RPC
	*******************************************************************************/
	void CActorChannel::RecvRPC( const RPC_Source& rpc, const neteng::Packet& packet )
	{
		if ( EnsureNot( !m_Actor ) )
		{
			RPCQueueData data;
			data.source = rpc;
			data.packet = packet;

			m_RPCQueue.push( data );
			return;
		}

		m_Actor->ReceiveRPC( rpc, packet );
	}

	/**	Add Peer
	*******************************************************************************/
	void CActorChannel::AddPeer( peer_hash peer )
	{
		if ( m_Peers.find( peer ) != m_Peers.end() )
			return;

		Debug_Log( "ActorChannel[%d] added peer %d", m_ChannelId, peer );

#if defined(GAME_SERVER)
		// Send add actor message
		if ( !m_Peers.empty() && IsServer() )
		{
			// Send to already added peers
			{
				neteng::Packet addPkt( nullptr, 0 );
				addPkt.PushHeader( peer );
				addPkt.PushHeader<unsigned char>( 1 );
				addPkt.PushHeader<channel_id>( m_ChannelId );
				addPkt.PushHeader( GameMessage::Channel_AddPeers );

				for ( peer_hash hash : m_Peers )
					m_Network->SendPeer( addPkt, PacketType::Reliable, hash );
			}

			{
				CSerializer s;
				s.Serialize<unsigned char>( (unsigned char)m_Peers.size() );
				for ( peer_hash hash : m_Peers )
					s.Serialize( hash );

				neteng::Packet addPkt( s.Data(), s.Size() );
				addPkt.PushHeader<channel_id>( m_ChannelId );
				addPkt.PushHeader( GameMessage::Channel_AddPeers );

				m_Network->SendPeer( addPkt, PacketType::Reliable, peer );
			}
		}

		m_Peers.emplace( peer );

		// Serialize and send net actor
		CSerializer serializer;
		m_Actor->Serialize( serializer );

		neteng::Packet pkt( serializer.Data(), serializer.Size() );
		pkt.PushHeader( m_ChannelId );
		pkt.PushHeader( GameMessage::Channel_Actor );

		Debug_Log( "Actor Channel [%d] sending actor to %d", m_ChannelId, peer );

		m_Network->SendPeer( pkt, neteng::PacketType::Reliable, peer );
#else
		// Clients don't really do anything, just add it
		if ( !IsServer() )
		{
			m_Peers.emplace( peer );
			return;
		}
#endif
	}

	/**	Add Peers
	*******************************************************************************/
	void CActorChannel::AddPeers( peer_hash* peers, size_t count )
	{
		for ( size_t i = 0; i < count; i++ )
			AddPeer( peers[i] );
	}

	/**	Remove Peer
	*******************************************************************************/
	void CActorChannel::RemovePeer( peer_hash peer )
	{
		m_Peers.erase( peer );
		Debug_Log( "ActorChannel[%d] removed peer %d", m_ChannelId, peer );

#if defined(GAME_SERVER)
		// Send add actor message (to others)
		if ( !m_Peers.empty() )
		{
			neteng::Packet rmvPacket( nullptr, 0 );
			rmvPacket.PushHeader( peer );
			rmvPacket.PushHeader<char>( 1 );
			rmvPacket.PushHeader<channel_id>( m_ChannelId );
			rmvPacket.PushHeader( GameMessage::Channel_RemovePeers );

			for ( peer_hash hash : m_Peers )
				m_Network->SendPeer( rmvPacket, PacketType::Reliable, hash );
		}
#endif
	}

	/**	Remove Peers
	*******************************************************************************/
	void CActorChannel::RemovePeers( peer_hash * peers, size_t count )
	{
		for ( size_t i = 0; i < count; i++ )
			RemovePeer( peers[i] );
	}
}
