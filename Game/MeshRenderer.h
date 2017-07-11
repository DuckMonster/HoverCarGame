#pragma once
#include "Component.h"
#include "VertexBufferType.h"

class CMeshRenderer : public CComponent
{
public:
	CMeshRenderer( class CActor* actor );

	void OnInit( ) override;
	void OnDestroy( ) override;
	void OnRender( const SRenderData& data ) override;

	void SetMeshDataSource( class CMeshDataSource* source ) { m_DataSource = source; }
	CMeshDataSource* GetMeshDataSource( ) const { return m_DataSource; }

	void LoadTextureFromFile( const char* path );

private:
	GLuint	m_Shader_PN = -1;
	GLuint	m_Shader_PNT = -1;

	GLuint	m_Texture = -1;
	bool	m_UseTexture = false;

	class CMeshDataSource* m_DataSource = nullptr;
};