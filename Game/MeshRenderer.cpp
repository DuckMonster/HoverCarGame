#include "stdafx.h"
#include "MeshRenderer.h"
#if !defined(GAME_SERVER)

#include "GLUtils.h"
#include "Transform.h"
#include "Time.h"
#include "Material.h"
#include "Scene.h"

using namespace std;
using namespace glm;
using namespace Resource;

/**	On Init
*******************************************************************************/
void CMeshRenderer::OnInit()
{
	CComponent::OnInit();
}

/**	On Destroy
*******************************************************************************/
void CMeshRenderer::OnDestroy()
{
	CComponent::OnDestroy();
}

/**	On Render
*******************************************************************************/
void CMeshRenderer::OnRender( const SRenderInfo& info )
{
	if ( !Ensure( m_Material && m_MeshResource ) )
		return;

	//--------------------------------------------------- Should this render?
	if ( info.Phase == RenderPhase::Deferred && m_Material->m_RenderMode == MaterialRenderMode::Forward ||
		info.Phase == RenderPhase::Forward && m_Material->m_RenderMode == MaterialRenderMode::Deferred ||
		info.Phase == RenderPhase::Shadow && !m_Material->m_CastsShadow )
	{
		return;
	}

	//--------------------------------------------------- Setup shader
	GLuint shader = m_Material->GetProgramHandle();
	if ( !Ensure( shader != -1 ) )
		return;

	if ( m_Material->HasTexture() )
		glBindTexture( GL_TEXTURE_2D, m_Material->GetTexture() );

	glUseProgram( shader );

	GLuint u_Camera = glGetUniformLocation( shader, "u_Camera" );
	GLuint u_Model = glGetUniformLocation( shader, "u_Model" );

	glUniformMatrix4fv( u_Camera, 1, false, value_ptr( info.CameraMatrix ) );
	glUniformMatrix4fv( u_Model, 1, false, value_ptr( Transform()->GetMatrix() ) );

	//--------------------------------------------------- Setup vertex data
	const CMeshResource::RenderData& data = m_MeshResource->GetRenderData( m_MeshIndex );
	glBindVertexArray( data.vao );
	glDrawElements( GL_TRIANGLES, data.vertCount, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );
}

/**	Set
*******************************************************************************/
void CMeshRenderer::Set( const std::string & resource, size_t index )
{
	SetMeshResource( GetScene()->GetResourceBank()->GetMesh( resource ) );
	SetMeshIndex( index );
}
#endif