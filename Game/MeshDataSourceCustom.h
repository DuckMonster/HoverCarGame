#pragma once
#include "MeshDataSource.h"

class CMeshDataSourceCustom : public CMeshDataSource
{
private:
	struct SBufferAttributePair
	{
		GLuint	Attribute = -1;
		GLuint	Buffer = -1;
	};

public:
	CMeshDataSourceCustom( CActor* actor ) : CMeshDataSource( actor ) {}

	void OnInit( ) override;
	void OnDestroy( ) override;

	void BeginRender( CMeshRenderer* renderer, SMeshDrawVertData& info ) override;

	GLuint AddElementBuffer( );
	GLuint GetElementBuffer( ) const { return m_ElementBuffer; }

	GLuint AddBuffer( GLuint attribute, GLenum type, int size, int stride, int offset );
	GLuint GetBuffer( GLuint attribute ) const;

	void SetDrawCount( size_t drawCount ) { m_DrawCount = drawCount; }
	size_t GetDrawCount( ) const { return m_DrawCount; }

	void SetDrawMode( GLenum drawMode ) { m_DrawMode = drawMode; }
	GLenum GetDrawMode( ) const { return m_DrawMode; }

private:
	GLuint	m_ElementBuffer = -1;
	std::vector<SBufferAttributePair> m_Buffers;

	size_t	m_DrawCount = 0;
	GLenum	m_DrawMode = GL_TRIANGLES;
};