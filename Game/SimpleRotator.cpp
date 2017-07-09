#include "stdafx.h"
#include "SimpleRotator.h"
#include "Time.h"
#include "Transform.h"

/**	Constructor
*******************************************************************************/
CSimpleRotator::CSimpleRotator( CActor * actor ) : CComponent( actor )
{
}

/**	On Update
*******************************************************************************/
void CSimpleRotator::OnUpdate( const SUpdateData& data )
{
	Transform( )->AddLocalRotation( glm::angleAxis( data.Delta * m_Speed, m_Angle ) );
}