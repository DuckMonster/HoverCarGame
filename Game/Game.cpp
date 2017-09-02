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
	m_CurrentInput.SetKey( key, state );
}

/**	Button Event
*******************************************************************************/
void CGame::ButtonEvent( int index, int button, bool state )
{
	m_CurrentInput.SetButton( index - 1, button, state );
}

/**	Axis Event
*******************************************************************************/
void CGame::AxisEvent( int index, sf::Joystick::Axis axis, float value )
{
	if (abs( value ) < 0.15f)
		value = 0.f;

	m_CurrentInput.SetAxis( index - 1, axis, value );
}

/** Mouse Move Event
*******************************************************************************/
void CGame::MouseMoveEvent( int x, int y )
{
}

/**	Mouse Button Event
*******************************************************************************/
void CGame::MouseButtonEvent( int button )
{
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

	if (delta > 0.1f)
		delta = 0.f;

	//--------------------------------------------------- Do Frame
	BeginFrame( delta );
	UpdateFrame( delta );
	RenderFrame( );
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
void CGame::RenderFrame( )
{
	//m_Deferred.RenderScene( m_Scene );

	glClearColor( 0.05f, 0.05f, 0.05f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );

	m_Deferred.RenderScene( m_Scene );
	//m_Forward.Render( m_Scene, 0.f );
	//m_Scene->Render( delta );
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