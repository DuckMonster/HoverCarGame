#pragma once
#include "Resource.h"

struct aiNode;
struct aiScene;
class CActor;

namespace Resource
{
	class CMeshResource : public CResource
	{
		struct MeshNode
		{
			std::string name;
			std::vector<size_t> meshes;
			glm::mat4 transform;

			MeshNode* parent;
			std::vector<MeshNode*> children;
		};

	public:
		struct RenderData
		{
			GLuint vao;
			GLuint vertBuffer;
			GLuint elemBuffer;
			GLuint vertCount;
		};

	public:
		CMeshResource( CResourceBank* bank ) : CResource( bank ) {}

		void Load( const std::string& path );
		void Reload() override;
		void Destroy() override;

		const RenderData& GetRenderData( size_t index ) { return m_Meshes[index]; }

		CActor* Instantiate( CActor* root );

	private:
		std::string m_FilePath;
		std::vector<RenderData> m_Meshes;
		MeshNode* m_RootNode = nullptr;

		CActor* InstantiateRecursive( CActor* root, MeshNode* node );

		MeshNode* LoadNodesRecursive( MeshNode* root, aiNode* node, const glm::mat4& transform = glm::mat4(1.f));
		void DestroyNodesRecursive( MeshNode* node );
		void LoadMeshes( const aiScene* scene );

	};
}