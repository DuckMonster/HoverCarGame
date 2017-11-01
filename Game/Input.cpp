#include "stdafx.h"
#include "Input.h"
using namespace glm;

#if !defined(GAME_SERVER)
namespace Input
{
	/**	Constructor
	*******************************************************************************/
	CInput::CInput()
	{
		memset( m_KeyData, 0, sizeof( m_KeyData ) );
		m_MousePosition = vec2( 0.f );
		memset( m_MouseButtonData, 0, sizeof( m_KeyData ) );
		memset( m_KeyData, 0, sizeof( m_KeyData ) );
		memset( m_AxisData, 0, sizeof( m_AxisData ) );
		memset( m_ButtonData, 0, sizeof( m_ButtonData ) );
	}

	/**	Copy Constructor
	*******************************************************************************/
	CInput::CInput( const CInput && other )
	{
		CopyData( other );
	}

	/**	Copy Data
	*******************************************************************************/
	void CInput::CopyData( const CInput& other )
	{
		if ( !m_DirtyFlags )
			return;

		if ( m_DirtyFlags & (int)DirtyType::Keyboard )
			memcpy( m_KeyData, other.m_KeyData, sizeof( m_KeyData ));
		if ( m_DirtyFlags & (int)DirtyType::MousePos )
			m_MousePosition = other.m_MousePosition;
		if ( m_DirtyFlags & (int)DirtyType::MouseBtn )
			memcpy( m_MouseButtonData, other.m_MouseButtonData, sizeof( m_MouseButtonData ) );
		if ( m_DirtyFlags & (int)DirtyType::JoyButton )
			memcpy( m_ButtonData, other.m_ButtonData, sizeof( m_ButtonData ) );
		if ( m_DirtyFlags & (int)DirtyType::JoyAxis )
			memcpy( m_AxisData, other.m_AxisData, sizeof( m_AxisData ) );

		m_DirtyFlags = 0;
	}

	/**	Is Key Pressed
	*******************************************************************************/
	bool CInput::KeyPressed( sf::Keyboard::Key key ) const
	{
		if ( m_Previous == nullptr )
			return false;

		return m_KeyData[key] && !m_Previous->m_KeyData[key];
	}

	/**	Is Key Released
	*******************************************************************************/
	bool CInput::KeyReleased( sf::Keyboard::Key key ) const
	{
		if ( m_Previous == nullptr )
			return false;

		return !m_KeyData[key] && m_Previous->m_KeyData[key];
	}

	/**	Get Mouse Position
	*******************************************************************************/
	vec2 CInput::MousePosition() const
	{
		return m_MousePosition;
	}

	/**	Get Mouse Button
	*******************************************************************************/
	bool CInput::MouseButton( int button ) const
	{
		return m_MouseButtonData[button];
	}

	/**	Is Mouse Button Pressed
	*******************************************************************************/
	bool CInput::MouseButtonPressed( int button ) const
	{
		if ( m_Previous == nullptr )
			return false;

		return m_MouseButtonData[button] && !m_Previous->m_MouseButtonData[button];
	}

	/**	Is Mouse Button Released
	*******************************************************************************/
	bool CInput::MouseButtonReleased( int button ) const
	{
		if ( m_Previous == nullptr )
			return false;

		return !m_MouseButtonData[button] && m_Previous->m_MouseButtonData[button];
	}

	/**	Is Button Pressed
	*******************************************************************************/
	bool CInput::ButtonPressed( int index, int button ) const
	{
		if ( m_Previous == nullptr )
			return false;

		return m_ButtonData[index][button] && !m_Previous->m_ButtonData[index][button];
	}

	/**	Is Button Released
	*******************************************************************************/
	bool CInput::ButtonReleased( int index, int button ) const
	{
		if ( m_Previous == nullptr )
			return false;

		return !m_ButtonData[index][button] && m_Previous->m_ButtonData[index][button];
	}
}
#endif