#include "stdafx.h"
#include "MeshDataSourceResource.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Time.h"

using namespace glm;
using namespace std;

namespace
{
	template<typename TypeIn, typename TypeOut>
	TypeOut ConvertVector( const TypeIn& vec )
	{
		TypeOut result;

		for (int i = 0; i < result.length( ); i++)
			result[i] = vec[i];

		return result;
	}
}

/**	On Init
*******************************************************************************/
void CMeshDataSourceResource::OnInit( )
{
	CMeshDataSource::OnInit();

	// Gen buffers
	glGenBuffers( (int)EVertexBufferType::TypeCount, m_DataBuffers );

	// Bind attributes
	glBindVertexArray( GetVertexObject( ) );

	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );

	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[0] );
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, 0, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[1] );
	glVertexAttribPointer( 1, 3, GL_FLOAT, false, 0, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, m_DataBuffers[2] );
	glVertexAttribPointer( 2, 2, GL_FLOAT, false, 0, 0 );
}

/**	On Destroy
*******************************************************************************/
void CMeshDataSourceResource::OnDestroy( )
{
	CMeshDataSource::OnDestroy();
	glDeleteBuffers( (int)EVertexBufferType::TypeCount, m_DataBuffers );
}

/**	Load Resource
*******************************************************************************/
void CMeshDataSourceResource::LoadResource( const char* path )
{
	using namespace Assimp;

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
	glBindBuffer( GL_ARRAY_BUFFER, GetBuffer(EVertexBufferType::Position) );
	glBufferData( GL_ARRAY_BUFFER, dataPosition.size( ) * sizeof( vec3 ), &dataPosition[0], GL_STATIC_DRAW );

	// Normal
	if (!dataNormal.empty( ))
	{
		glBindBuffer( GL_ARRAY_BUFFER, GetBuffer( EVertexBufferType::Normal) );
		glBufferData( GL_ARRAY_BUFFER, dataNormal.size( ) * sizeof( vec3 ), &dataNormal[0], GL_STATIC_DRAW );
	}

	// TexCoord
	if (!dataTex.empty( ))
	{
		glBindBuffer( GL_ARRAY_BUFFER, GetBuffer( EVertexBufferType::TexCoord ) );
		glBufferData( GL_ARRAY_BUFFER, dataTex.size( ) * sizeof( vec2 ), &dataTex[0], GL_STATIC_DRAW );
	}

	// Elements
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, GetBuffer( EVertexBufferType::Element ) );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, dataElement.size( ) * sizeof( size_t ), &dataElement[0], GL_STATIC_DRAW );

	m_DrawCount = dataElement.size( );

	Print_Log( "\"%s\" Loaded (%f s)", path, startTime.Elapsed( ) );
}

/**	Begin Render
*******************************************************************************/
void CMeshDataSourceResource::BeginRender( CMeshRenderer* renderer, SMeshDrawVertData& outInfo )
{
	CMeshDataSource::BeginRender(renderer, outInfo);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, GetBuffer( EVertexBufferType::Element ) );

	outInfo.UseElements = true;
	outInfo.DrawMode = GL_TRIANGLES;
	outInfo.Count = m_DrawCount;
	outInfo.Offset = 0;
}