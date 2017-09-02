#include "stdafx.h"
#include "Scene.h"
#include "Game.h"
#include "Actor.h"
#include "Component.h"
#include "GLUtils.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "MeshDataSourceResource.h"
#include "MeshDataSourceCustom.h"
#include "Material.h"
#include "File.h"
#include "DirectionalLight.h"

using namespace std;
using namespace glm;

namespace
{
	CActor* cube = nullptr;
	CActor* infiniFloor = nullptr;

	CMeshRenderer* renderer = nullptr;
	CMeshDataSourceResource* source1 = nullptr;
	CMeshDataSourceResource* source2 = nullptr;

	CMaterial* material = nullptr;
	CMaterial* floorMaterial = nullptr;
}

/**	Constructor
*******************************************************************************/
CScene::CScene( CGame* game ) :
	m_Game( game ),
	m_RootActor( nullptr )
{
	material = new CMaterial( );
	material->LoadFromFile( "../Assets/Shader/Deferred/D_default" );
	floorMaterial = new CMaterial( );
	floorMaterial->LoadFromFile( "../Assets/Shader/Deferred/D_infinite_floor" );

	m_RootActor = new CActor( this );
	m_RootActor->SetName( "ROOT" );
	m_RootActor->Init( );

	{
		cube = m_RootActor->SpawnActor( "Cube" );

		renderer = cube->AddComponent<CMeshRenderer>( );
		source1 = cube->AddComponent<CMeshDataSourceResource>( );

		renderer->SetMaterial( material );
		renderer->SetMeshDataSource( source1 );
		source1->LoadResource( "../Assets/UnitCube.fbx" );

		cube->AttachScript( []( CActor* actor, const SUpdateInfo& info )
		{
			float angle = -18.235f * info.Delta;
			float yPos = sin( CTime::TotalElapsed( ) * 0.8f ) * 0.6f;

			actor->Transform( )->AddRotationEuler( vec3( 0.f, angle, 0.f ) );
			actor->Transform( )->SetLocalPosition( vec3( 0.f, 1.5f + yPos, 0.f ) );
		} );
	}

	{
		CActor* cube2 = m_RootActor->SpawnActor( "Bouncy Cube" );
		cube2->Transform( )->SetLocalPosition( vec3( 1.5f, 4.f, 0.f ) );

		CMeshRenderer* rend = cube2->AddComponent<CMeshRenderer>( );
		CMeshDataSourceResource* src = cube2->AddComponent<CMeshDataSourceResource>( );

		rend->SetMaterial( material );
		rend->SetMeshDataSource( src );
		src->LoadResource( "../Assets/UnitSphere.fbx" );

		cube2->AttachScript( []( CActor* actor, const SUpdateInfo& info )
		{
			static float velo = 0.f;
			static const float GRAV = -8.f;

			static float jump_timer = 0.f;

			jump_timer += info.Delta;
			if (jump_timer > 5.f)
			{
				velo = 5.f;
				jump_timer = 0.f;
			}

			float yPos = actor->Transform( )->GetLocalPosition( ).y - 0.5f;

			velo += GRAV * info.Delta;
			if (yPos + velo * info.Delta < 0.f)
				velo *= -0.6f;

			yPos += velo * info.Delta;

			vec3 newPos = actor->Transform( )->GetLocalPosition( );
			newPos.y = yPos + 0.5f;

			actor->Transform( )->SetLocalPosition( newPos );
		} );
	}

	infiniFloor = m_RootActor->SpawnActor( "Infinite Floor" );
	{
		CMeshRenderer* renderer = infiniFloor->AddComponent<CMeshRenderer>( );
		CMeshDataSourceCustom* source = infiniFloor->AddComponent<CMeshDataSourceCustom>( );

		renderer->SetMeshDataSource( source );
		renderer->SetMaterial( floorMaterial );
		source->SetDrawCount( 4 );

		float positions[] ={
			-1.f, 0.f, -1.f,
			1.f, 0.f, -1.f,
			1.f, 0.f, 1.f,
			-1.f, 0.f, 1.f
		};

		GLuint pos = source->AddBuffer( 0, GL_FLOAT, 3, 0, 0 );

		glBindBuffer( GL_ARRAY_BUFFER, pos );
		glBufferData( GL_ARRAY_BUFFER, sizeof( positions ), positions, GL_STATIC_DRAW );

		source->SetDrawMode( GL_QUADS );
	}

	infiniFloor->AttachScript( []( CActor* actor, const SUpdateInfo& info )
	{
		vec3 cubePosition = cube->Transform( )->GetPosition( );
		actor->Transform( )->SetPosition( vec3( cubePosition.x, 0.f, cubePosition.z ) );
	} );

	// Setup camera
	CActor* cameraRoot = m_RootActor->SpawnActor( "Camera Root" );
	CActor* camera = cameraRoot->SpawnActor( "My Camera" );
	camera->AddComponent<CCamera>( );

	camera->Transform( )->SetLocalPosition( vec3( -5.f, 1.f, 5.f ) );
	camera->Transform( )->LookAt( vec3( ) );

	cameraRoot->AttachScript( []( CActor* actor, const SUpdateInfo& info )
	{
		float angle = 12.f * info.Delta;
		actor->Transform( )->AddRotationEuler( vec3( 0.f, angle, 0.f ) );
	} );

	// Setup light
	CActor* light = m_RootActor->SpawnActor( "Directional Light" );
	light->AddComponent<CDirectionalLight>( );

	light->Transform( )->SetLocalPosition( vec3( 9.f, 7.f, 5.f ) );
	light->Transform( )->LookAt( vec3( ) );

	PrintScene( );
}

/**	Destructor
*******************************************************************************/
CScene::~CScene( )
{
}

/**	Update
*******************************************************************************/
void CScene::Update( float delta )
{
	SUpdateInfo data{ delta, m_Game->GetInput( ) };
	m_RootActor->Update( data );
}

/**	Get Active Camera
*******************************************************************************/
CCamera* CScene::GetActiveCamera( )
{
	// Find active camera
	if (!m_ActiveCamera)
	{
		m_ActiveCamera = m_RootActor->FindComponent<CCamera>( );

		// Spawn one if necessary
		if (!m_ActiveCamera)
		{
			m_ActiveCamera = m_RootActor->SpawnActor( "Main Camera" )->AddComponent<CCamera>( );
			m_ActiveCamera->Transform( )->SetLocalPosition( vec3( 1.f, 1.f, 1.f ) * 10.f );
			m_ActiveCamera->Transform( )->LookAt( vec3( 0.f ) );
		}
	}

	return m_ActiveCamera;
}

/**	Print Scene
*******************************************************************************/
void CScene::PrintScene( )
{
	string sceneString;

	sceneString += "\n-- SCENE --\n";
	GetActorString( m_RootActor, 1, sceneString );

	Print_Log( "%s", sceneString.c_str( ) );
}

/**	Get Actor String
*******************************************************************************/
void CScene::GetActorString( CActor* actor, int indent, string& outString )
{
	struct
	{
		void IndentString( string& str, int indent, bool mark = false )
		{
			for (int i=0; i < indent; i++)
			{
				if (mark && i == indent - 1)
					str += "-  ";
				else
					str += "   ";
			}
		}
	} func;

	func.IndentString( outString, indent, true );
	outString += actor->GetName( ) + '\n';
	for (size_t i = 0; i < actor->NumComponents( ); i++)
	{
		CComponent* comp = actor->GetComponent( i );
		func.IndentString( outString, indent, false );

		outString += comp->GetName( ) + '\n';
	}

	if (actor->NumScripts( ) > 0)
	{
		func.IndentString( outString, indent, false );
		outString += "( " + to_string( actor->NumScripts( ) ) + " ) scripts\n";
	}

	for (size_t i = 0; i < actor->NumChildren( ); i++)
		GetActorString( actor->GetChild( i ), indent + 1, outString );
}
