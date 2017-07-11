#include "stdafx.h"
#include "MeshDataSource.h"

/**	On Init
*******************************************************************************/
void CMeshDataSource::OnInit( )
{
	glGenVertexArrays( 1, &m_VertexObject );
}

/**	On Destroy
*******************************************************************************/
void CMeshDataSource::OnDestroy( )
{
	if (m_VertexObject == -1)
		return;

	glDeleteVertexArrays( 1, &m_VertexObject );
	m_VertexObject = -1;
}

/**	Begin Render
*******************************************************************************/
void CMeshDataSource::BeginRender( CMeshRenderer * renderer, SMeshDrawInfo& outInfo )
{
	glBindVertexArray( m_VertexObject );
	outInfo = SMeshDrawInfo( );
}

/**	End Render
*******************************************************************************/
void CMeshDataSource::EndRender( CMeshRenderer * renderer )
{
	glBindVertexArray( 0 );
}
