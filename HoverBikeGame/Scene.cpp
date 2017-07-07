#include "stdafx.h"
#include "Scene.h"
#include "Game.h"
#include "Actor.h"

CScene::CScene( CGame* game ) :
	m_Game( game ),
	m_RootActor( nullptr )
{
	m_RootActor = new CActor( this );
	m_RootActor->SetName( "ROOT" );
	m_RootActor->Init( );

	m_RootActor->SpawnActor( "MyActor", m_RootActor );
}

CScene::~CScene( )
{
}

void CScene::Update( float delta )
{
}

void CScene::Render( float delta )
{
}
