#pragma once
namespace Input
{
#if !defined(GAME_SERVER)
	class CInput
	{
		enum class DirtyType : int
		{
			Keyboard = 1 << 0,
			MousePos = 1 << 1,
			MouseBtn = 1 << 2,
			JoyButton = 1 << 3,
			JoyAxis = 1 << 4
		};

	public:
		CInput();
		CInput( const CInput&& other );

		void SetKey( sf::Keyboard::Key key, bool state ) { m_KeyData[key] = state; SetDirty( DirtyType::Keyboard ); }
		void SetAxis( int joyIndex, sf::Joystick::Axis axis, float value ) { m_AxisData[joyIndex][axis] = value; SetDirty( DirtyType::JoyAxis ); }
		void SetButton( int joyIndex, int button, bool state ) { m_ButtonData[joyIndex][button] = state; SetDirty( DirtyType::JoyButton ); }

		void SetMousePosition( const glm::vec2& mousePosition ) { m_MousePosition = mousePosition; SetDirty( DirtyType::MousePos ); }
		void SetMouseButton( int button, bool state ) { m_MouseButtonData[button] = state; SetDirty( DirtyType::MouseBtn ); }

		void SetPrevious( CInput* previous ) { m_Previous = previous; m_Previous->m_DirtyFlags = 0xFFFFFFFF; }
		void CopyData( const CInput& other );

		bool KeyDown( sf::Keyboard::Key key ) const { return m_KeyData[key]; }
		bool KeyPressed( sf::Keyboard::Key key ) const;
		bool KeyReleased( sf::Keyboard::Key key ) const;

		glm::vec2 MousePosition() const;
		bool MouseButton( int button ) const;
		bool MouseButtonPressed( int button ) const;
		bool MouseButtonReleased( int button ) const;

		bool ButtonDown( int index, int button ) const { return m_ButtonData[index][button]; }
		bool ButtonPressed( int index, int button ) const;
		bool ButtonReleased( int index, int button ) const;

		float AxisValue( int index, sf::Joystick::Axis axis ) const { return m_AxisData[index][axis]; }

	private:
		void SetDirty( DirtyType type ) { if (m_Previous) m_Previous->m_DirtyFlags |= (int)type; }

		int m_DirtyFlags = 0;

		bool m_KeyData[sf::Keyboard::KeyCount];
		glm::vec2 m_MousePosition;
		bool m_MouseButtonData[sf::Mouse::ButtonCount];

		bool m_ButtonData[sf::Joystick::ButtonCount][sf::Joystick::Count];
		float m_AxisData[sf::Joystick::AxisCount][sf::Joystick::Count];

		CInput* m_Previous = nullptr;
	};
#endif
}