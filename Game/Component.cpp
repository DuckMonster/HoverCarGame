#include "stdafx.h"
#include "Component.h"
#include "Actor.h"

/**	Constructor
*******************************************************************************/
CComponent::CComponent( CActor * actor, const char* name ) :
	m_Actor( actor ),
	m_Name( name )
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
	Print_Log( "\"%s\" added to \"%s\".", GetName( ).c_str( ), GetActor( )->GetName( ).c_str( ) );
}

/**	On Destroy
*******************************************************************************/
void CComponent::OnDestroy( )
{
	Print_Log( "Component \"%s\" destroyed.", GetName( ).c_str( ) );
}

/**	On Update
*******************************************************************************/
void CComponent::OnUpdate( const SUpdateInfo& info )
{
}

/**	On Render
*******************************************************************************/
void CComponent::OnRender( const SRenderInfo& info )
{
}

/**	Get Transform
*******************************************************************************/
CTransform* CComponent::Transform( ) const
{
	return m_Actor->Transform( );
}

