#include "stdafx.h"
#include "Scene.h"

#include <net/BinaryStream.h>
#include "Game.h"
#include "Actor.h"
#include "Component.h"
#include "GLUtils.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "File.h"
#include "DirectionalLight.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "StringUtils.h"
#include "Random.h"
#include "ActorChannel.h"
#include "ServerBall.h"
#include "MeshResource.h"
#include "CustomRenderer.h"
#include "CharacterMovement.h"
#include "SimpleAIController.h"

using namespace std;
using namespace glm;
using namespace Network;
using namespace neteng;
using namespace Resource;

namespace
{
	CActor* cube = nullptr;
	CActor* infiniFloor = nullptr;

	CMeshRenderer* renderer = nullptr;

	CMaterial* material = nullptr;
	CMaterial* floorMaterial = nullptr;
	CMaterial* aimMaterial = nullptr;
}

namespace
{
	enum class SceneNetMessage : char
	{
		SpawnNetActor
	};
}


/**	Constructor
*******************************************************************************/
CScene::CScene( CGame* game ) :
	m_Game( game ),
	m_RootActor( nullptr )
{
	m_InitData.LoadFile( "../Assets/Data/init.json" );

	material = new CMaterial();
	material->LoadFromFile( "../Assets/Shader/Deferred/D_default" );
	floorMaterial = new CMaterial();
	floorMaterial->LoadFromFile( "../Assets/Shader/Deferred/D_infinite_floor" );
	aimMaterial = new CMaterial();
	aimMaterial->LoadFromFile( "../Assets/Shader/Forward/F_aim_cone" );

	m_RootActor = new CActor( this );
	m_RootActor->SetName( "ROOT" );
	m_RootActor->Init();

	CMeshResource* sceneMesh = m_ResourceBank.GetMesh( "../Assets/MyScene.fbx" );
	CActor* meshActor = sceneMesh->Instantiate( m_RootActor );

	m_Physics.SetRoot( m_RootActor );

	infiniFloor = m_RootActor->SpawnActor( "Infinite Floor" );
	{
		CCustomRenderer* renderer = infiniFloor->AddComponent<CCustomRenderer>();

		renderer->SetMaterial( floorMaterial );
		renderer->SetDrawCount( 4 );

#if !defined(GAME_SERVER)
		float positions[] = {
			-1.f, 0.f, -1.f,
			1.f, 0.f, -1.f,
			1.f, 0.f, 1.f,
			-1.f, 0.f, 1.f
		};

		GLuint pos = renderer->AddBuffer();
		renderer->BindAttribute( 0, 0, GL_FLOAT, 3, 0, 0 );

		glBindBuffer( GL_ARRAY_BUFFER, pos );
		glBufferData( GL_ARRAY_BUFFER, sizeof( positions ), positions, GL_STATIC_DRAW );

		renderer->SetDrawMode( GL_QUADS );
#endif
	}

	// Setup camera
	CActor* cameraRoot = m_RootActor->SpawnActor( "Camera Root" );
	cameraRoot->AddComponent<CCameraController>();
	CActor* camera = cameraRoot->SpawnActor( "My Camera" );
	camera->AddComponent<CCamera>();

	camera->Transform()->SetPosition( vec3( 0.f, 5.f, 5.f ) );
	camera->Transform()->LookAt( vec3() );

	infiniFloor->Transform()->SetParent( cameraRoot->Transform() );

	// Setup light
	CActor* light = cameraRoot->SpawnActor( "Directional Light" );
	light->AddComponent<CDirectionalLight>();

	light->Transform()->SetPosition( vec3( 9.f, 7.f, 5.f ) );
	light->Transform()->LookAt( vec3() );

	// TEMP AI
#if defined(GAME_SERVER)
	CActor* aiActor = SpawnActor( "Simple AI" );
	aiActor->AddComponent<CCharacterMovement>();
	aiActor->AddComponent<CSimpleAIController>();

	RegisterNetActor( aiActor );
#endif
}

/**	Destructor
*******************************************************************************/
CScene::~CScene()
{
}

/**	Update
*******************************************************************************/
void CScene::Update( float delta )
{
#if !defined(GAME_SERVER)
	SUpdateInfo data( m_Game->GetInput() );
	data.Delta = delta;

	if ( data.Input.KeyPressed( sf::Keyboard::P ) )
		PrintScene( data.Input.KeyDown( sf::Keyboard::LShift ) );
#else
	SUpdateInfo data{ delta };
#endif

	if ( m_InitData.LoadIfChanged() )
	{
		m_RootActor->ReLoadComponents();
	}

	data.Phase = UpdatePhase::Pre;
	m_RootActor->Update( data );
	data.Phase = UpdatePhase::Main;
	m_RootActor->Update( data );
	data.Phase = UpdatePhase::Post;
	m_RootActor->Update( data );

}

/**	Get Active Camera
*******************************************************************************/
CCamera* CScene::GetActiveCamera()
{
	// Find active camera
	if ( !m_ActiveCamera )
	{
		m_ActiveCamera = m_RootActor->FindComponent<CCamera>();

		// Spawn one if necessary
		if ( !m_ActiveCamera )
		{
			m_ActiveCamera = m_RootActor->SpawnActor( "Main Camera" )->AddComponent<CCamera>();
			m_ActiveCamera->Transform()->SetPosition( vec3( 1.f, 1.f, 1.f ) * 10.f );
			m_ActiveCamera->Transform()->LookAt( vec3( 0.f ) );
		}
	}

	return m_ActiveCamera;
}

/**	Print Scene
*******************************************************************************/
void CScene::PrintScene( bool printComponents )
{
	string sceneString;

	sceneString += "\n-- SCENE --\n";
	GetActorString( m_RootActor, printComponents, 1, sceneString );
	Debug_Log( "%s", sceneString.c_str() );
}

/**	Evaluate Path
*******************************************************************************/
CActor* CScene::EvaluatePath( const CPath& path ) const
{
	return m_RootActor->EvaluatePath( path );
}

/**	Spawn Actor
*******************************************************************************/
CActor* CScene::SpawnActor( const std::string& name )
{
	return m_RootActor->SpawnActor( name );
}

/**	On Net Message
*******************************************************************************/
void CScene::OnNetMessage( const neteng::Packet& pkt )
{
	BinaryReader reader( pkt.Data() );
	SceneNetMessage msg = reader.Read<SceneNetMessage>();

	switch ( msg )
	{
	case SceneNetMessage::SpawnNetActor:
		break;
	}
}

#if defined(GAME_SERVER)
/**	Spawn Net Actor
*******************************************************************************/
void CScene::SpawnBouncyBall()
{
	CActor* ball = SpawnActor( "Bouncy Ball" );
	ball->Transform()->SetPosition( vec3( CRandom::Value( -10.f, 10.f ), 5.f, CRandom::Value( -10.f, 10.0f ) ) );
	ball->Transform()->SetScale( vec3( CRandom::Value( 0.5f, 2.f ) ) );
	ball->AddComponent<CServerBall>()->Set( CRandom::Value( 0.1f, 0.5f ), CRandom::Value( 5.f, 20.f ) );

	RegisterNetActor( ball );
}

/**	On Player Connected
*******************************************************************************/
void CScene::OnPlayerConnected( peer_hash hash )
{
	for ( CActor* netActor : m_NetActorList )
		netActor->ReplicateTo( hash );

	SpawnPlayer( hash );
	SpawnBouncyBall();
}

/**	On Player Disconnected
*******************************************************************************/
void CScene::OnPlayerDisconnected( neteng::peer_hash hash )
{
	CActor* playerActor = m_PlayerMap[hash];
	channel_id channel = playerActor->GetChannel()->GetId();

	m_PlayerMap[hash] = nullptr;
	m_NetActorList.erase( find( m_NetActorList.begin(), m_NetActorList.end(), playerActor ) );
	m_NetActorMap.erase( channel );

	playerActor->Destroy();
}

/**	Spawn Player
*******************************************************************************/
void CScene::SpawnPlayer( neteng::peer_hash hash )
{
	CPlayerController* player;

	{
		cube = m_RootActor->SpawnActor( "Player" );
		cube->AddComponent<CCharacterMovement>();
		player = cube->AddComponent<CPlayerController>();
		CActor* spawnActor = m_RootActor->Find( "spawn" );

		if ( spawnActor )
			player->Transform()->SetPosition( spawnActor->Transform()->GetPosition() );
		else
			Debug_Log( "No spawn for player!" );

		RegisterNetActor( cube );

		player->SendPossess( hash );
	}

	m_PlayerMap[hash] = player->GetActor();
}

/**	Register Net Actor
*******************************************************************************/
void CScene::RegisterNetActor( CActor * actor )
{
	channel_id chnlId = m_LastNetId++;

	actor->Replicate( chnlId );

	// Add all current peers
	for ( CNetwork::peer_iterator it = GetGame()->GetNetwork()->PeerBegin(); it != GetGame()->GetNetwork()->PeerEnd(); ++it )
		actor->ReplicateTo( *it );

	m_NetActorList.push_back( actor );
	m_NetActorMap[chnlId] = actor;
}
#endif

/**	Get Actor String
*******************************************************************************/
void CScene::GetActorString( CActor* actor, bool printComponents, int indent, string& outString )
{
	struct
	{
		void IndentString( string& str, int indent, bool mark = false )
		{
			for ( int i = 0; i < indent; i++ )
			{
				if ( mark && i == indent - 1 )
					str += "-  ";
				else
					str += "   ";
			}
		}
	} func;

	func.IndentString( outString, indent, true );
	outString += actor->GetName().str() + '\n';
	if ( printComponents )
	{
		for ( size_t i = 0; i < actor->NumComponents(); i++ )
		{
			CComponent* comp = actor->GetComponentAt( i );
			func.IndentString( outString, indent, false );

			outString += comp->GetName();
			outString += " " + comp->GetDisplayString() + '\n';
		}

		if ( actor->NumScripts() > 0 )
		{
			func.IndentString( outString, indent, false );
			outString += "( " + to_string( actor->NumScripts() ) + " ) scripts\n";
		}
	}

	for ( size_t i = 0; i < actor->NumChildren(); i++ )
		GetActorString( actor->GetChild( i ), printComponents, indent + 1, outString );
}
