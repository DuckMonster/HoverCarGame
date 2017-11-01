#include "stdafx.h"
#include "MeshResource.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Actor.h"
#include "Time.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"
#include "Collider.h"

using namespace Assimp;
using namespace std;
using namespace glm;

namespace
{
	CMaterial material;
}

namespace Resource
{
	/**	Load
	*******************************************************************************/
	void CMeshResource::Load( const std::string& path )
	{
		m_FilePath = path;

		Debug_Log( "Loading mesh \"%s\"", path.c_str() );
		CTimePoint startTime;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile( path, aiProcess_OptimizeMeshes | aiProcess_Triangulate );

		if ( scene == nullptr )
		{
			Debug_Log( "Failed to open %s: %s", path.c_str(), importer.GetErrorString() );
			return;
		}

		LoadMeshes( scene );
		m_RootNode = LoadNodesRecursive( nullptr, scene->mRootNode, mat4() );

		Debug_Log( "\"%s\" Loaded (%f s)", path.c_str(), startTime.Elapsed() );
	}

	/**	Reload
	*******************************************************************************/
	void CMeshResource::Reload()
	{
	}

	/**	Destroy
	*******************************************************************************/
	void CMeshResource::Destroy()
	{
#if !defined(GAME_SERVER)
		// Destroy all meshes
		for ( RenderData& data : m_Meshes )
		{
			glDeleteVertexArrays( 1, &data.vao );
			glDeleteBuffers( 2, &data.vertBuffer );
		}

		m_Meshes.clear();
#endif

		// Destroy nodes
		DestroyNodesRecursive( m_RootNode );
		m_RootNode = nullptr;
	}

	/**	Instantiate
	*******************************************************************************/
	CActor* CMeshResource::Instantiate( CActor* root )
	{
		material.LoadFromFile( "../Assets/Shader/Deferred/D_default" );
		return InstantiateRecursive( root, m_RootNode );
	}

	/**	Instantiate Recursive
	*******************************************************************************/
	CActor* CMeshResource::InstantiateRecursive( CActor* root, MeshNode* node )
	{
		CActor* newActor = root->SpawnActor( node->name );
		newActor->Transform()->SetTransform( node->transform );

		for ( size_t mesh : node->meshes )
		{
			CMeshRenderer* renderer = newActor->AddComponent<CMeshRenderer>();
			renderer->SetMeshResource( this );
			renderer->SetMeshIndex( mesh );
			renderer->SetMaterial( &material );

			CCollider* collider = newActor->AddComponent<CCollider>();
			collider->SetIsStatic( true );
		}

		for ( MeshNode* nodeChild : node->children )
			InstantiateRecursive( newActor, nodeChild );

		return newActor;
	}

	/**	Load Nodes Recursive
	*******************************************************************************/
	CMeshResource::MeshNode* CMeshResource::LoadNodesRecursive( MeshNode* root, aiNode* node, const glm::mat4& transform/*= mat4(1.f)*/ )
	{
		std::string name( node->mName.C_Str() );

		// This is just FBX data that we don't want
		if ( name.find( "$AssimpFbx$" ) != string::npos )
		{
			// Calculate transform without this node, and move on
			mat4 localTransform = transform * glm::transpose( *(mat4*)&node->mTransformation );

			for ( size_t i = 0; i < node->mNumChildren; i++ )
			{
				MeshNode* child = LoadNodesRecursive( root, node->mChildren[i], localTransform );

				if ( child )
					return child;
			}

			return nullptr;
		}

		MeshNode* newNode = new MeshNode;
		newNode->name = node->mName.C_Str();
		newNode->parent = root;
		newNode->transform = transform * glm::transpose( *(mat4*)&node->mTransformation );

		for ( size_t i = 0; i < node->mNumMeshes; i++ )
		{
			newNode->meshes.push_back( node->mMeshes[i] );
		}

		for ( size_t i = 0; i < node->mNumChildren; i++ )
		{
			MeshNode* child = LoadNodesRecursive( newNode, node->mChildren[i], transform );

			if ( child )
				newNode->children.push_back( child );
		}

		return newNode;
	}

	/**	Destroy Nodes Recursive
	*******************************************************************************/
	void CMeshResource::DestroyNodesRecursive( MeshNode* node )
	{
		for ( MeshNode* child : node->children )
			DestroyNodesRecursive( child );

		delete node;
	}

	/**	Load Meshes
	*******************************************************************************/
	void CMeshResource::LoadMeshes( const aiScene* scene )
	{
#if !defined(GAME_SERVER)
		struct Vertex
		{
			vec3 position;
			vec3 normal;
			vec2 uv;
		};

		vector<Vertex> data_vertex;
		vector<size_t> data_index;

		for ( size_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++ )
		{
			aiMesh* inMesh = scene->mMeshes[meshIndex];

			//--------------------------------------------------- Load Vertex Data
			for ( size_t vertIndex = 0; vertIndex < inMesh->mNumVertices; ++vertIndex )
			{
				Vertex vert;
				vert.position = *(vec3*)&inMesh->mVertices[vertIndex];

				if ( inMesh->mNormals != nullptr )
					vert.normal = *(vec3*)&inMesh->mNormals[vertIndex];

				if ( inMesh->mTextureCoords[0] != nullptr )
					vert.uv = *(vec2*)&inMesh->mTextureCoords[0][vertIndex];

				data_vertex.push_back( vert );
			}

			//--------------------------------------------------- Load Index Data
			for ( size_t faceIndex = 0; faceIndex < inMesh->mNumFaces; ++faceIndex )
			{
				const aiFace& face = inMesh->mFaces[faceIndex];

				if ( !EnsureMsg( face.mNumIndices == 3, "Mesh \"%s\" has a face with %d vertices", m_FilePath.c_str(), face.mNumIndices ) )
					continue;

				for ( size_t vertIndex = 0; vertIndex < face.mNumIndices; vertIndex++ )
					data_index.push_back( face.mIndices[vertIndex] );
			}

			//--------------------------------------------------- Create buffers for this mesh
			RenderData data;
			data.vertCount = data_index.size();

			// VAO
			glGenVertexArrays( 1, &data.vao );
			glBindVertexArray( data.vao );

			// Buffers
			glGenBuffers( 2, &data.vertBuffer );

			// Indicies
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data.elemBuffer );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( size_t ) * data_index.size(), &data_index[0], GL_STATIC_DRAW );

			// Vert data
			glBindBuffer( GL_ARRAY_BUFFER, data.vertBuffer );
			glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * data_vertex.size(), &data_vertex[0], GL_STATIC_DRAW );

			// Bind attributes
			glEnableVertexAttribArray( 0 );
			glEnableVertexAttribArray( 1 );
			glEnableVertexAttribArray( 2 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( Vertex ), 0 );
			glVertexAttribPointer( 1, 3, GL_FLOAT, false, sizeof( Vertex ), (void*)( sizeof( vec3 ) ) );
			glVertexAttribPointer( 2, 2, GL_FLOAT, false, sizeof( Vertex ), (void*)( sizeof( vec3 ) + sizeof( vec2 ) ) );

			data_vertex.clear();
			data_index.clear();

			m_Meshes.push_back( data );
		}
#endif
	}
}