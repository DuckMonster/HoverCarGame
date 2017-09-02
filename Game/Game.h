#pragma once
#include <SFML/Window/Keyboard.hpp>
#include "Input.h"
#include "Time.h"
#include "Deferred.h"
#include "Forward.h"

class CGame
{
public:
	CGame( );
	~CGame( );

	void KeyEvent( sf::Keyboard::Key key, bool state );
	void ButtonEvent( int index, int button, bool state );
	void AxisEvent( int index, sf::Joystick::Axis axis, float value );
	void MouseMoveEvent( int x, int y );
	void MouseButtonEvent( int button );

	void OnViewportChange( int width, int height );
	const glm::ivec2& GetViewport( ) const { return m_ViewportSize; };
	const Input::CInput& GetInput( ) const { return m_CurrentInput; }

	void OnFrame( );

private:
	void BeginFrame( float delta );
	void UpdateFrame( float delta );
	void RenderFrame( );
	void EndFrame( float delta );

private:
	bool				m_IsFirstFrame = true;
	CTimePoint			m_PrevFrameTime;

	class CScene*		m_Scene = nullptr;
	glm::ivec2			m_ViewportSize;

	Input::CInput		m_CurrentInput;
	Input::CInput		m_PreviousInput;

	CDeferred			m_Deferred;
	CForward			m_Forward;
};