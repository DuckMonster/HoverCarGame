#pragma once
#include "MeshDataSource.h"

class CMeshDataSourceResource : public CMeshDataSource
{
public:
	CMeshDataSourceResource( CActor* actor ) : CMeshDataSource( actor ) {}
	void OnInit( ) override;
	void OnDestroy( ) override;

	void LoadResource( const char* path );

	GLuint GetBuffer( EVertexBufferType type ) const { return m_DataBuffers[(int)type]; }

	void BeginRender( class CMeshRenderer* renderer, SMeshDrawVertData& outInfo ) override;

private:
	size_t	m_DrawCount;

	GLuint	m_DataBuffers[(int)EVertexBufferType::TypeCount];
};