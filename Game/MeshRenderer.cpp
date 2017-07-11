#include "stdafx.h"
#include "MeshRenderer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL/SOIL.h>
#include "ShaderUtils.h"
#include "MeshDataSource.h"
#include "Transform.h"
#include "Time.h"

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
CMeshRenderer::CMeshRenderer( CActor * actor ) : CComponent( actor )
{
}

/**	On Init
*******************************************************************************/
void CMeshRenderer::OnInit( )
{
	CComponent::OnInit( );
	m_Shader_PN = ShaderUtils::CreateShaderFromFile( "../Assets/Shader/default_PN" );
	m_Shader_PNT = ShaderUtils::CreateShaderFromFile( "../Assets/Shader/default_PNT" );
}

/**	On Destroy
*******************************************************************************/
void CMeshRenderer::OnDestroy( )
{
	CComponent::OnDestroy( );
	glDeleteProgram( m_Shader_PN );
	glDeleteProgram( m_Shader_PNT );
}

/**	On Render
*******************************************************************************/
void CMeshRenderer::OnRender( const SRenderData& data )
{
	if (m_DataSource == nullptr)
		return;

	//--------------------------------------------------- Setup shader
	GLuint shader = m_UseTexture ? m_Shader_PNT : m_Shader_PN;
	if (m_UseTexture)
		glBindTexture( GL_TEXTURE_2D, m_Texture );

	glUseProgram( shader );

	GLuint u_Camera = glGetUniformLocation( shader, "u_Camera" );
	GLuint u_Model = glGetUniformLocation( shader, "u_Model" );

	glUniformMatrix4fv( u_Camera, 1, false, value_ptr( data.CameraMatrix ) );
	glUniformMatrix4fv( u_Model, 1, false, value_ptr( Transform( )->GetMatrix( ) ) );

	//--------------------------------------------------- Setup vertex data
	SMeshDrawInfo drawInfo;

	m_DataSource->BeginRender( this, drawInfo );

	if (drawInfo.UseElements)
		glDrawElements( drawInfo.DrawMode, drawInfo.Count, GL_UNSIGNED_INT, (void*)drawInfo.Offset );
	else
		glDrawArrays( drawInfo.DrawMode, drawInfo.Offset, drawInfo.Count );

	m_DataSource->EndRender( this );
}

/**	Load Texture From File
*******************************************************************************/
void CMeshRenderer::LoadTextureFromFile( const char* path )
{
	Print_Log( "Loading texture \"%s\"", path );

	int width, height;
	unsigned char* image = SOIL_load_image( path, &width, &height, nullptr, SOIL_LOAD_RGB );

	if (image == nullptr)
	{
		Print_Log( "Failed to load \"%s\"", path );
		return;
	}

	glGenTextures( 1, &m_Texture );
	glBindTexture( GL_TEXTURE_2D, m_Texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	m_UseTexture = true;

	SOIL_free_image_data( image );
}