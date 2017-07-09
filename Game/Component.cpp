#include "stdafx.h"
#include "Component.h"
#include "Actor.h"

/**	Constructor
*******************************************************************************/
CComponent::CComponent( CActor * actor )
	: m_Actor( actor )
{
}

/**	Destructor
*******************************************************************************/
CComponent::~CComponent( )
{
}

/**	On Init
*******************************************************************************/
void CComponent::OnInit( )
{
	Print_Log( "Component added to %s.", GetActor( )->GetName( ).c_str() );
}

/**	On Destroy
*******************************************************************************/
void CComponent::OnDestroy( )
{
	Print_Log( "Some component destroyed." );
}

/**	On Update
*******************************************************************************/
void CComponent::OnUpdate( const SUpdateData& data )
{
}

/**	On Render
*******************************************************************************/
void CComponent::OnRender( const SRenderData& data )
{
}

/**	Get Transform
*******************************************************************************/
CTransform* CComponent::Transform( ) const
{
	return m_Actor->Transform( );
}
