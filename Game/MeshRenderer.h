#pragma once
#include "Component.h"
#include "MeshResource.h"

class CMeshRenderer : public CComponent
{
public:
	CMeshRenderer::CMeshRenderer( CActor * actor ) : CComponent( actor ) { Register<CMeshRenderer>(); }

#if defined(GAME_SERVER)
	void Set( const std::string& resource, size_t index ) {}
#else
	void OnInit() override;
	void OnDestroy() override;
	void OnRender( const SRenderInfo& info ) override;

	void Set( const std::string& resource, size_t index );
#endif

	void SetMeshResource( Resource::CMeshResource* resource ) { m_MeshResource = resource; }
	Resource::CMeshResource* GetMeshResource() const { return m_MeshResource; }

	void SetMeshIndex( size_t index ) { m_MeshIndex = index; }
	size_t GetMeshIndex() const { return m_MeshIndex; }

	void SetMaterial( class CMaterial* material ) { m_Material = material; }
	CMaterial* GetMaterial() const { return m_Material; }

private:
	CMaterial* m_Material = nullptr;
	Resource::CMeshResource* m_MeshResource = nullptr;
	size_t m_MeshIndex = 0;
};