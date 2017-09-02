#include "stdafx.h"
#include "MeshDataSourceCustom.h"

/**	On Init
*******************************************************************************/
void CMeshDataSourceCustom::OnInit( )
{
	CMeshDataSource::OnInit();
}

/**	On Destroy
*******************************************************************************/
void CMeshDataSourceCustom::OnDestroy( )
{
	CMeshDataSource::OnDestroy();

	for (size_t i = 0; i < m_Buffers.size(); --i)
		glDeleteBuffers( 1, &m_Buffers[i].Buffer );

	if (m_ElementBuffer != -1)
		glDeleteBuffers( 1, &m_ElementBuffer );

	m_Buffers.clear( );
	m_ElementBuffer = -1;
}

/**	Begin Render
*******************************************************************************/
void CMeshDataSourceCustom::BeginRender( CMeshRenderer * renderer, SMeshDrawVertData & info )
{
	CMeshDataSource::BeginRender(renderer, info);

	info.DrawMode = m_DrawMode;
	info.Count = m_DrawCount;
	info.Offset = 0;
	info.UseElements = m_ElementBuffer != -1;
}

/**	Add Element Buffer
*******************************************************************************/
GLuint CMeshDataSourceCustom::AddElementBuffer( )
{
	if (m_ElementBuffer != -1)
		return m_ElementBuffer;

	glGenBuffers( 1, &m_ElementBuffer );
	return m_ElementBuffer;
}

/**	Add Buffer
*******************************************************************************/
GLuint CMeshDataSourceCustom::AddBuffer( GLuint attribute, GLenum type, int size, int stride, int offset )
{
	SBufferAttributePair pair;
	pair.Attribute = attribute;

	m_Buffers.push_back( pair );

	glGenBuffers( 1, &pair.Buffer );

	// Bind buffer to attribute
	glBindVertexArray( GetVertexObject( ) );
	glBindBuffer( GL_ARRAY_BUFFER, pair.Buffer );
	glEnableVertexAttribArray( attribute );
	glVertexAttribPointer( attribute, size, type, false, stride, (void*)offset );
	glBindVertexArray( 0 );

	return pair.Buffer;
}

/**	Get Buffer
*******************************************************************************/
GLuint CMeshDataSourceCustom::GetBuffer( GLuint attribute ) const
{
	for (size_t i = 0; i < m_Buffers.size(); ++i)
	{
		if (m_Buffers[i].Attribute == attribute)
			return m_Buffers[i].Buffer;
	}

	return -1;
}
