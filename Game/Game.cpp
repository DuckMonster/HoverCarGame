#include "stdafx.h"
#include "Game.h"
#include "Scene.h"
#include "Transform.h"
#include "Gizmo.h"

/**	Constructor
*******************************************************************************/
CGame::CGame( )
{
	m_Scene = new CScene( this );
	
	CGizmo::InitGizmos( );
	m_CurrentInput.SetPrevious( &m_PreviousInput );
}

/**	Destructor
*******************************************************************************/
CGame::~CGame( )
{
	CGizmo::DestroyGizmos( );
}

/**	KeyEvent
*******************************************************************************/
void CGame::KeyEvent( sf::Keyboard::Key key, bool state )
{
	m_CurrentInput.Set( key, state );
}

/**	On Viewport Change
*******************************************************************************/
void CGame::OnViewportChange( int width, int height )
{
	m_ViewportSize.x = width;
	m_ViewportSize.y = height;

	// Update GL
	glViewport( 0, 0, width, height );
}

/**	On Frame
*******************************************************************************/
void CGame::OnFrame( )
{
	using namespace std::chrono;

	//--------------------------------------------------- Calculate delta time
	if (m_IsFirstFrame)
	{
		m_PrevFrameTime = CTimePoint( );
		m_IsFirstFrame = false;
	}

	CTimePoint nowPoint;

	// Get Delta
	float delta = nowPoint - m_PrevFrameTime;
	m_PrevFrameTime = nowPoint;

	if (delta > 0.8f)
		delta = 0.f;

	//--------------------------------------------------- Do Frame
	BeginFrame( delta );
	UpdateFrame( delta );
	RenderFrame( delta );
	EndFrame( delta );
}

/**	Begin Frame
*******************************************************************************/
void CGame::BeginFrame( float delta )
{
}

/**	Update Frame
*******************************************************************************/
void CGame::UpdateFrame( float delta )
{
	m_Scene->Update( delta );
}

/**	Render Frame
*******************************************************************************/
void CGame::RenderFrame( float delta )
{
	glClearColor( 0.05f, 0.05f, 0.05f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );

	m_Scene->Render( delta );
}

/**	End Frame
*******************************************************************************/
void CGame::EndFrame( float delta )
{
	size_t worldUpdates = CTransform::sm_WorldUpdates,
		localUpdate = CTransform::sm_LocalUpdates;

	//Print_Log( "Local Updates: %d; World Updates: %d", localUpdate, worldUpdates );

	CTransform::sm_WorldUpdates = CTransform::sm_LocalUpdates = 0;

	// Copy input to previous
	m_PreviousInput.CopyData( m_CurrentInput );
}