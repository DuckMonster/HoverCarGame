#include "stdafx.h"
#include "MeshRenderer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL/SOIL.h>
#include "GLUtils.h"
#include "MeshDataSource.h"
#include "Transform.h"
#include "Time.h"
#include "Material.h"

using namespace std;
using namespace glm;

namespace
{
	const char* VERT = "\
#version 330 core\n\
layout(location = 0) in vec3 a_Vertex;\
\
uniform mat4 u_Camera;\
uniform mat4 u_Model;\
\
void main() {\
	gl_Position = u_Camera * u_Model * vec4(a_Vertex, 1.0);\
}\
";

	const char* FRAG = "\
#version 330 core\n\
out vec4 o_Color;\
void main() {\
	o_Color = vec4(1.0, 0.0, 0.0, 1.0);\
}\
";
}

/**	Constructor
*******************************************************************************/
CMeshRenderer::CMeshRenderer( CActor * actor ) : CComponent( actor, "Mesh Renderer" )
{
}

/**	On Init
*******************************************************************************/
void CMeshRenderer::OnInit( )
{
	CComponent::OnInit( );
}

/**	On Destroy
*******************************************************************************/
void CMeshRenderer::OnDestroy( )
{
	CComponent::OnDestroy( );
}

/**	On Render
*******************************************************************************/
void CMeshRenderer::OnRender( const SRenderInfo& info )
{
	if (m_DataSource == nullptr)
		return;

	//--------------------------------------------------- Setup shader
	GLuint shader = -1;

	if (info.UseShaderOverride)
		shader = info.ShaderOverride;
	else if (m_Material)
		shader = m_Material->GetProgramHandle( );
	else // No shaders
		return;

	if (m_Material->HasTexture( ))
		glBindTexture( GL_TEXTURE_2D, m_Material->GetTexture( ) );

	glUseProgram( shader );

	GLuint u_Camera = glGetUniformLocation( shader, "u_Camera" );
	GLuint u_Model = glGetUniformLocation( shader, "u_Model" );

	glUniformMatrix4fv( u_Camera, 1, false, value_ptr( info.CameraMatrix ) );
	glUniformMatrix4fv( u_Model, 1, false, value_ptr( Transform( )->GetMatrix( ) ) );

	//--------------------------------------------------- Setup vertex data
	SMeshDrawVertData drawInfo;

	m_DataSource->BeginRender( this, drawInfo );

	if (drawInfo.UseElements)
		glDrawElements( drawInfo.DrawMode, drawInfo.Count, GL_UNSIGNED_INT, (void*)drawInfo.Offset );
	else
		glDrawArrays( drawInfo.DrawMode, drawInfo.Offset, drawInfo.Count );

	m_DataSource->EndRender( this );
}