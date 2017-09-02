#pragma once
#include "Component.h"
#include "VertexBufferType.h"

class CMeshRenderer : public CComponent
{
public:
	CMeshRenderer( class CActor* actor );

	void OnInit( ) override;
	void OnDestroy( ) override;
	void OnRender( const SRenderInfo& info ) override;

	void SetMeshDataSource( class CMeshDataSource* source ) { m_DataSource = source; }
	CMeshDataSource* GetMeshDataSource( ) const { return m_DataSource; }

	void SetMaterial( class CMaterial* material ) { m_Material = material; }
	CMaterial* GetMaterial( ) const { return m_Material; }

private:
	class CMeshDataSource*	m_DataSource = nullptr;
	class CMaterial*		m_Material = nullptr;
};