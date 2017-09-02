#pragma once
#include "Component.h"
#include "VertexBufferType.h"

struct SMeshDrawVertData
{
	GLenum DrawMode = GL_TRIANGLES;
	size_t Count = 0;
	size_t Offset = 0;
	bool UseElements = false;
};

class CMeshDataSource : public CComponent
{
public:
	CMeshDataSource( CActor* actor ) : CComponent( actor, "Mesh Data Source" ) {}
	void OnInit( ) override;
	void OnDestroy( ) override;

	GLuint GetVertexObject( ) const { return m_VertexObject; }

	virtual void BeginRender( class CMeshRenderer* renderer, SMeshDrawVertData& outInfo );
	virtual void EndRender( class CMeshRenderer* renderer );

private:
	GLuint		m_VertexObject = -1;
};