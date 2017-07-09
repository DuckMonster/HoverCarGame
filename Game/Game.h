#pragma once
#include <SFML/Window/Keyboard.hpp>
#include "Input.h"
#include "Time.h"

class CGame
{
public:
	CGame( );
	~CGame( );

	void KeyEvent( sf::Keyboard::Key key, bool state );

	void OnViewportChange( int width, int height );
	const glm::ivec2& GetViewport( ) const { return m_ViewportSize; };
	const CInput& GetInput( ) const { return m_CurrentInput; }

	void OnFrame( );

private:
	void BeginFrame( float delta );
	void UpdateFrame( float delta );
	void RenderFrame( float delta );
	void EndFrame( float delta );

private:
	bool				m_IsFirstFrame = true;
	CTimePoint			m_PrevFrameTime;

	class CScene*		m_Scene = nullptr;
	glm::ivec2			m_ViewportSize;

	CInput				m_CurrentInput;
	CInput				m_PreviousInput;
};