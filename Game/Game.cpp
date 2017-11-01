#include "stdafx.h"
#include "Game.h"
#include "Scene.h"
#include "Transform.h"
#include "Gizmo.h"

using namespace neteng;

/**	Constructor
*******************************************************************************/
CGame::CGame()
{
	CComponentFactory::RegisterAllComponents();

	m_Scene = new CScene( this );
	CDataStructure netData( "../Assets/Data/net.json" );

#if !defined(GAME_SERVER)
	CGizmo::InitGizmos();
	m_CurrentInput.SetPrevious( &m_PreviousInput );

	netData.Read( "/QuitOnDisconnect", m_QuitOnDisconnect );
#endif

	if ( !m_Network.StartUp( this, netData ) )
	{
		EnsureMsg( false, "Failed to start network" );
		if ( Network::IsServer() )
			QuitGame();
	}
}

/**	Destructor
*******************************************************************************/
CGame::~CGame()
{
#if !defined(GAME_SERVER)
	CGizmo::DestroyGizmos();
#endif
}

#if !defined(GAME_SERVER)
/**	KeyEvent
*******************************************************************************/
void CGame::KeyEvent( sf::Keyboard::Key key, bool state )
{
	m_CurrentInput.SetKey( key, state );

	if ( key == sf::Keyboard::T && state )
	{
		m_DebugDraw = !m_DebugDraw;
	}
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
	if ( abs( value ) < 0.15f )
		value = 0.f;

	m_CurrentInput.SetAxis( index - 1, axis, value );
}

/** Mouse Move Event
*******************************************************************************/
void CGame::MouseMoveEvent( int x, int y )
{
	glm::vec2 norm = glm::vec2( x, y ) / glm::vec2( m_ViewportSize );
	norm = norm * 2.f - 1.f;
	norm.y *= -1.f;

	m_CurrentInput.SetMousePosition( norm );
}

/**	Mouse Button Event
*******************************************************************************/
void CGame::MouseButtonEvent( int button, bool state )
{
	m_CurrentInput.SetMouseButton( button, state );
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
#endif

void CGame::OnNetEvent( const NetEvent& event )
{
#if defined(GAME_SERVER)
	switch ( event.type )
	{
	case NetEventType::PeerConnected:
		Debug_Log( "Client (%d) connected", event.peer );
		m_Scene->OnPlayerConnected( event.peer );
		break;

	case NetEventType::PeerDisconnected:
		Debug_Log( "Client (%d) disconnected", event.peer );
		m_Scene->OnPlayerDisconnected( event.peer );
		break;
	}
#else
	switch ( event.type )
	{
	case NetEventType::PeerConnected:
		Debug_Log( "Connected, hash: %d", event.peer );
		break;

	case NetEventType::PeerDisconnected:
		Debug_Log( "Disconnected!", event.peer );

		if ( m_QuitOnDisconnect )
			QuitGame();
		break;
	}
#endif
}

/**	On Frame
*******************************************************************************/
bool CGame::OnFrame()
{
	using namespace std::chrono;

	//--------------------------------------------------- Calculate delta time
	if ( m_IsFirstFrame )
	{
		m_PrevFrameTime = CTimePoint();
		m_IsFirstFrame = false;
	}

	CTimePoint nowPoint;

	// Get Delta
	float delta = nowPoint - m_PrevFrameTime;
	m_PrevFrameTime = nowPoint;

	if ( delta > 0.1f )
		delta = 0.1f;

	//--------------------------------------------------- Do Frame
	BeginFrame( delta );
	UpdateFrame( delta );
#if !defined(GAME_SERVER)
	RenderFrame();
#endif
	EndFrame( delta );

	return m_Active;
}

/**	Begin Frame
*******************************************************************************/
void CGame::BeginFrame( float delta )
{
	m_Network.PollEvents();
}

/**	Update Frame
*******************************************************************************/
void CGame::UpdateFrame( float delta )
{
	m_Scene->Update( delta );
}

#if !defined(GAME_SERVER)
/**	Render Frame
*******************************************************************************/
void CGame::RenderFrame()
{
	//m_Deferred.RenderScene( m_Scene );

	glClearColor( 0.05f, 0.05f, 0.05f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );

	m_Deferred.RenderScene( m_Scene );
	//m_Forward.Render( m_Scene, 0.f );
	//m_Scene->Render( delta );
}
#endif

/**	End Frame
*******************************************************************************/
void CGame::EndFrame( float delta )
{
	size_t worldUpdates = CTransform::sm_WorldUpdates,
		localUpdate = CTransform::sm_LocalUpdates;

	CTransform::sm_WorldUpdates = CTransform::sm_LocalUpdates = 0;

#if !defined(GAME_SERVER)
	// Copy input to previous
	m_PreviousInput.CopyData( m_CurrentInput );
#endif
}