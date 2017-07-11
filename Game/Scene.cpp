#include "stdafx.h"
#include "Scene.h"
#include "Game.h"
#include "Actor.h"
#include "Component.h"
#include "ShaderUtils.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "SimpleRotator.h"
#include "CarController.h"
#include "CameraController.h"
#include "MeshDataSourceResource.h"

using namespace glm;

static CActor* testActor = nullptr;
static CActor* testActor2 = nullptr;

static CMeshRenderer* renderer = nullptr;
static CMeshDataSourceResource* source1 = nullptr;
static CMeshDataSourceResource* source2 = nullptr;

/**	Constructor
*******************************************************************************/
CScene::CScene( CGame* game ) :
	m_Game( game ),
	m_RootActor( nullptr )
{
	m_RootActor = new CActor( this );
	m_RootActor->SetName( "ROOT" );
	m_RootActor->Init( );

	float deg, rad;
	deg = 180.f;
	rad = Math::Deg2Rad * deg;

	Print_Log( "%f deg = %f rad (%f)", deg, rad, pi<float>( ) );
	deg = 90.f;
	rad = Math::Deg2Rad * deg;
	Print_Log( "%f deg = %f rad (%f)", deg, rad, pi<float>( ) / 2.f );
	deg = 270.f;
	rad = Math::Deg2Rad * deg;
	Print_Log( "%f deg = %f rad (%f)", deg, rad, pi<float>( ) * 1.5f );
	deg = 360.f;
	rad = Math::Deg2Rad * deg;
	Print_Log( "%f deg = %f rad (%f)", deg, rad, pi<float>( ) * 2.f );

	for (size_t i = 0; i < 1; i++)
	{
		CActor* newActor = m_RootActor->SpawnActor( "MyActor", m_RootActor );

		Print_Log( "Component num: %d", newActor->GetComponentCount( ) );

		newActor->AddComponent<CComponent>( );
		Print_Log( "Component num: %d", newActor->GetComponentCount( ) );
		newActor->RemoveComponent<CComponent>( );

		Print_Log( "Component num: %d", newActor->GetComponentCount( ) );

		CComponent* comp = newActor->AddComponent<CComponent>( );
		Print_Log( "Component num: %d", newActor->GetComponentCount( ) );
		newActor->RemoveComponent( comp );

		Print_Log( "Component num: %d", newActor->GetComponentCount( ) );

		newActor->AddComponent<CComponent>( );
		newActor->AddComponent<CComponent>( );
		newActor->AddComponent<CComponent>( );

		newActor->Destroy( );
		delete newActor;
	}

	CActor* mesh = m_RootActor->SpawnActor( "Mesh" );
	renderer = mesh->AddComponent<CMeshRenderer>( );
	source1 = mesh->AddComponent<CMeshDataSourceResource>( );
	source2 = mesh->AddComponent<CMeshDataSourceResource>( );
	renderer->SetMeshDataSource( source1 );
	source1->LoadResource( "../Assets/Ship.fbx" );
	source2->LoadResource( "../Assets/UnitSphere.fbx" );

	mesh->AddComponent<CCarController>( );

	CActor* camera = m_RootActor->SpawnActor( "My Camera" );
	camera->AddComponent<CCamera>( )->SetProjection( false );
	camera->AddComponent<CCameraController>( )->m_Target = mesh->Transform( );

	camera->Transform( )->SetLocalPosition( vec3( 1.f, 1.f, 1.f ) * 5.f );
	camera->Transform( )->LookAt( vec3( 0.f ) );
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
	SUpdateData data{ delta, m_Game->GetInput( ) };

	if (data.Input.KeyDown( sf::Keyboard::Space ))
		renderer->SetMeshDataSource( source2 );
	else
		renderer->SetMeshDataSource( source1 );

	m_RootActor->Update( data );
}

/**	Render
*******************************************************************************/
void CScene::Render( float delta )
{
	using namespace glm;

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

	ivec2 screenSize = m_Game->GetViewport( );
	float aspect = (float)screenSize.x / screenSize.y;
	m_ActiveCamera->SetAspect( aspect );

	SRenderData data{ delta, m_ActiveCamera->GetMatrix( ) };

	m_RootActor->Render( data );
}