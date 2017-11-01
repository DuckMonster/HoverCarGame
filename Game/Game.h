#pragma once
#include "Network.h"
#include "Input.h"
#include "Time.h"
#include "Deferred.h"
#include "Forward.h"

class CGame
{
public:
	CGame();
	~CGame();

#if !defined(GAME_SERVER)
	void KeyEvent( sf::Keyboard::Key key, bool state );
	void ButtonEvent( int index, int button, bool state );
	void AxisEvent( int index, sf::Joystick::Axis axis, float value );
	void MouseMoveEvent( int x, int y );
	void MouseButtonEvent( int button, bool state );

	void OnViewportChange( int width, int height );
	const glm::ivec2& GetViewport() const { return m_ViewportSize; };
	const Input::CInput& GetInput() const { return m_CurrentInput; }

	bool GetDebugDraw() const { return m_DebugDraw; }
#endif

	Network::CNetwork* GetNetwork() { return &m_Network; }
	class CScene* GetScene() const { return m_Scene; }

	void OnNetEvent( const neteng::NetEvent& event );
	bool OnFrame();

private:
	void BeginFrame( float delta );
	void UpdateFrame( float delta );

#if !defined(GAME_SERVER)
	void RenderFrame();
#endif

	void EndFrame( float delta );

	void QuitGame() { m_Active = false; }

private:
	Network::CNetwork	m_Network;
	bool				m_QuitOnDisconnect = false;

	bool				m_Active = true;

	bool				m_IsFirstFrame = true;
	CTimePoint			m_PrevFrameTime;

	class CScene*		m_Scene = nullptr;


#if !defined(GAME_SERVER)
	glm::ivec2			m_ViewportSize;

	Input::CInput		m_CurrentInput;
	Input::CInput		m_PreviousInput;

	CDeferred			m_Deferred;
	CForward			m_Forward;

	bool				m_DebugDraw = false;
#endif
};