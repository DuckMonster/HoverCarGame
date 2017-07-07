#include "stdafx.h"
#include "Game.h"
#include "Scene.h"

CGame::CGame( ) :
	m_Scene( nullptr )
{
	m_Scene = new CScene( this );
}

CGame::~CGame( )
{
}

void CGame::Update( float delta )
{
	m_Scene->Update( delta );
}

void CGame::Render( float delta )
{
	m_Scene->Render( delta );
}
