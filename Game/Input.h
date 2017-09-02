#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

namespace Input
{
	class CInput
	{
	public:
		CInput( );
		CInput( const CInput&& other );

		void SetKey( sf::Keyboard::Key key, bool state ) { m_KeyData[key] = state; m_Previous->m_IsDirty = true; }
		void SetAxis( int joyIndex, sf::Joystick::Axis axis, float value ) { m_AxisData[joyIndex][axis] = value; m_Previous->m_IsDirty = true; }
		void SetButton( int joyIndex, int button, bool state ) { m_ButtonData[joyIndex][button] = state; m_Previous->m_IsDirty = true; }

		void SetPrevious( CInput* previous ) { m_Previous = previous; m_Previous->m_IsDirty = true; }
		void CopyData( const CInput& other );

		bool KeyDown( sf::Keyboard::Key key ) const { return m_KeyData[key]; }
		bool KeyPressed( sf::Keyboard::Key key ) const;
		bool KeyReleased( sf::Keyboard::Key key ) const;

		bool ButtonDown( int index, int button ) const { return m_ButtonData[index][button]; }
		bool ButtonPressed( int index, int button ) const;
		bool ButtonReleased( int index, int button ) const;

		float AxisValue( int index, sf::Joystick::Axis axis ) const { return m_AxisData[index][axis]; }

	private:
		bool m_IsDirty = false;

		bool m_KeyData[sf::Keyboard::KeyCount];
		bool m_ButtonData[sf::Joystick::ButtonCount][sf::Joystick::Count];
		float m_AxisData[sf::Joystick::AxisCount][sf::Joystick::Count];

		CInput* m_Previous = nullptr;
	};
}