#pragma once
#include <SFML/Window/Keyboard.hpp>

class CInput
{
public:
	CInput( );
	CInput( const CInput&& other );

	void Set( sf::Keyboard::Key key, bool state ) { m_KeyData[key] = state; }
	void SetPrevious( CInput* previous ) { m_Previous = previous; }
	void CopyData( const CInput& other );

	bool KeyDown( sf::Keyboard::Key key ) const { return m_KeyData[key]; }
	bool KeyPressed( sf::Keyboard::Key key ) const;
	bool KeyReleased( sf::Keyboard::Key key ) const;

private:
	bool m_KeyData[sf::Keyboard::KeyCount];
	CInput* m_Previous = nullptr;
};