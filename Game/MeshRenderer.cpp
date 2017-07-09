#include "stdafx.h"
#include "MeshRenderer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL/SOIL.h>
#include "ShaderUtils.h"
#include "Transform.h"
#include "Time.h"

using namespace std;
using namespace glm;

namespace
{
	const float TEMP_VERT_DATA[] ={
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		0.f, 1.f, 0.f
	};

	const float TEMP_NORM_DATA[] ={
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f
	};

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

	template<typename TypeIn, typename TypeOut>
	TypeOut ConvertVector( const TypeIn& vec )
	{
		TypeOut result;

		for (int i = 0; i < result.length( ); i++)
			result[i] = vec[i];

		return result;
	}
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
	glGenVertexArrays( 1, &m_VertexObject );
	glBindVertexArray( m_VertexObject );
	glGenBuffers( BUFFER_COUNT, m_DataBuffers );

	//--------------------------------------------------- Temporary triangle
	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Position] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( TEMP_VERT_DATA ), TEMP_VERT_DATA, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Normal] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( TEMP_NORM_DATA ), TEMP_NORM_DATA, GL_STATIC_DRAW );
	m_DrawCount = 3;

	//--------------------------------------------------- Vertex Attributes
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );

	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Position] );
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, 0, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Normal] );
	glVertexAttribPointer( 1, 3, GL_FLOAT, false, 0, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::TexCoord] );
	glVertexAttribPointer( 2, 2, GL_FLOAT, false, 0, 0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Element] );

	glBindVertexArray( 0 );

	m_Shader_PN = ShaderUtils::CreateShaderFromFile( "../Assets/Shader/default_PN" );
	m_Shader_PNT = ShaderUtils::CreateShaderFromFile( "../Assets/Shader/default_PNT" );
}

/**	On Destroy
*******************************************************************************/
void CMeshRenderer::OnDestroy( )
{
	CComponent::OnDestroy( );
	glDeleteVertexArrays( 1, &m_VertexObject );
	glDeleteBuffers( BUFFER_COUNT, m_DataBuffers );
	glDeleteProgram( m_Shader_PN );
}

/**	On Render
*******************************************************************************/
void CMeshRenderer::OnRender( const SRenderData& data )
{
	GLuint shader = m_UseTexture ? m_Shader_PNT : m_Shader_PN;
	if (m_UseTexture)
		glBindTexture( GL_TEXTURE_2D, m_Texture );

	glUseProgram( shader );

	GLuint u_Camera = glGetUniformLocation( shader, "u_Camera" );
	GLuint u_Model = glGetUniformLocation( shader, "u_Model" );

	glUniformMatrix4fv( u_Camera, 1, false, value_ptr( data.CameraMatrix ) );
	glUniformMatrix4fv( u_Model, 1, false, value_ptr( Transform( )->GetMatrix( ) ) );

	glBindVertexArray( m_VertexObject );

	if (m_UseElementBuffer)
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Element] );
		glDrawElements( GL_TRIANGLES, m_DrawCount, GL_UNSIGNED_INT, 0 );
	}
	else
	{
		glDrawArrays( GL_TRIANGLES, 0, m_DrawCount );
	}
	glBindVertexArray( 0 );
}

/**	Load Mesh
*******************************************************************************/
void CMeshRenderer::LoadMeshFromFile( const char* path )
{
	using namespace Assimp;
	using namespace std;

	Print_Log( "Loading mesh \"%s\"", path );
	CTimePoint startTime;

	Importer importer;
	const aiScene* scene = importer.ReadFile( path, aiProcess_OptimizeMeshes | aiProcess_Triangulate );

	if (scene == nullptr)
	{
		Print_Log( "Failed to open %s", path );
		return;
	}

	//--------------------------------------------------- Load Mesh Data
	vector<vec3> dataPosition;
	vector<vec3> dataNormal;
	vector<vec2> dataTex;
	vector<size_t> dataElement;

	for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];

		for (size_t vertIndex = 0; vertIndex < mesh->mNumVertices; ++vertIndex)
		{
			dataPosition.push_back( ConvertVector<aiVector3D, vec3>( mesh->mVertices[vertIndex] ) );

			if (mesh->mNormals != nullptr)
				dataNormal.push_back( ConvertVector<aiVector3D, vec3>( mesh->mNormals[vertIndex] ) );

			if (mesh->mTextureCoords[0] != nullptr)
				dataTex.push_back( ConvertVector<aiVector3D, vec2>( mesh->mTextureCoords[0][vertIndex] ) );
		}

		for (size_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];

			for (size_t iIndex = 0; iIndex < face.mNumIndices; ++iIndex)
			{
				size_t index = face.mIndices[iIndex];
				dataElement.push_back( index );
			}
		}
	}

	//--------------------------------------------------- Upload to OpenGL
	// Position
	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Position] );
	glBufferData( GL_ARRAY_BUFFER, dataPosition.size( ) * sizeof( vec3 ), &dataPosition[0], GL_STATIC_DRAW );

	// Normal
	if (!dataNormal.empty( ))
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Normal] );
		glBufferData( GL_ARRAY_BUFFER, dataNormal.size( ) * sizeof( vec3 ), &dataNormal[0], GL_STATIC_DRAW );
	}

	// TexCoord
	if (!dataTex.empty( ))
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::TexCoord] );
		glBufferData( GL_ARRAY_BUFFER, dataTex.size( ) * sizeof( vec2 ), &dataTex[0], GL_STATIC_DRAW );
	}

	// Elements
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_DataBuffers[EDataBuffer::Element] );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, dataElement.size( ) * sizeof( size_t ), &dataElement[0], GL_STATIC_DRAW );

	m_DrawCount = dataElement.size( );

	Print_Log( "\"%s\" Loaded (%f s)", path, startTime.Elapsed( ) );
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