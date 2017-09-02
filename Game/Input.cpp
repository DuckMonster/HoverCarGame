#include "stdafx.h"
#include "Input.h"

namespace Input
{
	/**	Constructor
	*******************************************************************************/
	CInput::CInput( )
	{
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
		if (!m_IsDirty)
			return;

		memcpy( m_KeyData, other.m_KeyData, sizeof( m_KeyData ) );
		memcpy( m_AxisData, other.m_AxisData, sizeof( m_AxisData ) );
		memcpy( m_ButtonData, other.m_ButtonData, sizeof( m_ButtonData ) );

		m_IsDirty = false;
	}

	/**	Is Key Pressed
	*******************************************************************************/
	bool CInput::KeyPressed( sf::Keyboard::Key key ) const
	{
		if (m_Previous == nullptr)
			return false;

		return m_KeyData[key] && !m_Previous->m_KeyData[key];
	}

	/**	Is Key Released
	*******************************************************************************/
	bool CInput::KeyReleased( sf::Keyboard::Key key ) const
	{
		if (m_Previous == nullptr)
			return false;

		return !m_KeyData[key] && m_Previous->m_KeyData[key];
	}

	/**	Is Button Pressed
	*******************************************************************************/
	bool CInput::ButtonPressed( int index, int button ) const
	{
		if (m_Previous == nullptr)
			return false;

		return m_ButtonData[index][button] && !m_Previous->m_ButtonData[index][button];
	}

	/**	Is Button Released
	*******************************************************************************/
	bool CInput::ButtonReleased( int index, int button ) const
	{
		if (m_Previous == nullptr)
			return false;

		return !m_ButtonData[index][button] && m_Previous->m_ButtonData[index][button];
	}
}