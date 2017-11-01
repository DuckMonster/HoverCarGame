#pragma once
#include <unordered_map>
#include <net/BinaryStream.h>
#include <net/Hash.h>
#include "ResourceBank.h"
#include "Physics.h"
#include "DataStructure.h"
#include "Network.h"
#include "Path.h"

class CScene
{
public:
	CScene( class CGame* game );
	~CScene();

	void Update( float delta );

	class CActor* GetRootActor() const { return m_RootActor; }
	class CGame* GetGame() const { return m_Game; }
	class CCamera* GetActiveCamera();
	class CCamera* SetActiveCamera( CCamera* camera ) { m_ActiveCamera = camera; }

	const CDataStructure& GetInitData() const { return m_InitData; }

	CActor* SpawnActor( const std::string& name );

	void PrintScene( bool printComponents );
	CActor* EvaluatePath( const CPath& path ) const;

	Resource::CResourceBank* GetResourceBank() { return &m_ResourceBank; }
	Physics::CPhysics* GetPhysics() { return &m_Physics; }

	// Network
	void OnNetMessage( const neteng::Packet& pkt );

#if defined(GAME_SERVER)
	void SpawnBouncyBall();
	void OnPlayerConnected( neteng::peer_hash hash );
	void OnPlayerDisconnected( neteng::peer_hash hash );
	void SpawnPlayer( neteng::peer_hash hash );
	void RegisterNetActor( CActor* actor );
#endif

private:
	void GetActorString( CActor* actor, bool printComponents, int indent, std::string& outString );

	class CGame* const	m_Game;
	class CActor*	m_RootActor = nullptr;
	class CCamera*	m_ActiveCamera = nullptr;
	CDataStructure	m_InitData;

	Network::channel_id	m_LastNetId = 1;

	std::vector<CActor*> m_NetActorList;
	std::unordered_map<Network::channel_id, CActor*> m_NetActorMap;
	std::unordered_map<neteng::peer_hash, CActor*> m_PlayerMap;

	Physics::CPhysics m_Physics;
	Resource::CResourceBank m_ResourceBank;
};