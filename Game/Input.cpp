#include "stdafx.h"
#include "Input.h"

/**	Constructor
*******************************************************************************/
CInput::CInput( )
{
	memset( m_KeyData, 0, sizeof( m_KeyData ) );
}

/**	Copy Constructor
*******************************************************************************/
CInput::CInput( const CInput && other )
{
	CopyData( other );
}

/**	Copy Data
*******************************************************************************/
void CInput::CopyData( const CInput & other )
{
	memcpy( m_KeyData, other.m_KeyData, sizeof( m_KeyData ) );
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