#include "stdafx.h"
#include "CustomRenderer.h"

#if !defined(GAME_SERVER)
#include "Material.h"
#include "Transform.h"

/**	On Init
*******************************************************************************/
void CCustomRenderer::OnInit()
{
	CComponent::OnInit();
	glGenVertexArrays( 1, &m_VertexObject );
}

/**	On Destroy
*******************************************************************************/
void CCustomRenderer::OnDestroy()
{
	CComponent::OnDestroy();
	glDeleteVertexArrays( 1, &m_VertexObject );
	glDeleteBuffers( m_Buffers.size(), &m_Buffers[0] );

	m_Buffers.clear();
}

/**	On Render
*******************************************************************************/
void CCustomRenderer::OnRender( const SRenderInfo & info )
{
	CComponent::OnRender(info);

	if ( EnsureNot( !m_Material ) )
		return;

	//--------------------------------------------------- Should this render?
	if ( info.Phase == RenderPhase::Deferred && m_Material->m_RenderMode == MaterialRenderMode::Forward ||
		info.Phase == RenderPhase::Forward && m_Material->m_RenderMode == MaterialRenderMode::Deferred ||
		info.Phase == RenderPhase::Shadow && !m_Material->m_CastsShadow )
	{
		return;
	}

	// Setup shader
	glUseProgram( m_Material->GetProgramHandle() );
	m_Material->Set( "u_Camera", info.CameraMatrix );
	m_Material->Set( "u_Model", Transform()->GetMatrix() );

	//
	glBindVertexArray( m_VertexObject );
	glDrawArrays( m_DrawMode, 0, m_DrawCount );
	glBindVertexArray( 0 );
}

/**	Add Buffer
*******************************************************************************/
GLuint CCustomRenderer::AddBuffer()
{
	GLuint buffer;
	glGenBuffers( 1, &buffer );
	m_Buffers.push_back( buffer );

	return buffer;
}

/**	Add Attribute
*******************************************************************************/
void CCustomRenderer::BindAttribute( size_t bufferIndex, GLuint attrib, GLenum type, GLuint size, size_t stride, size_t offset )
{
	GLuint buffer = m_Buffers[bufferIndex];
	glBindVertexArray( m_VertexObject );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glEnableVertexAttribArray( attrib );
	glVertexAttribPointer( attrib, size, type, false, sizeof( float ) * stride, (void*)( sizeof( float ) * offset ) );
	glBindVertexArray( 0 );
}
#endif