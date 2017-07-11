#pragma once
#include "Component.h"
#include "VertexBufferType.h"

struct SMeshDrawInfo
{
	GLenum DrawMode = GL_TRIANGLES;
	size_t Count = 0;
	size_t Offset = 0;
	bool UseElements = false;
};

class CMeshDataSource : public CComponent
{
public:
	CMeshDataSource( CActor* actor ) : CComponent( actor ) {}
	void OnInit( ) override;
	void OnDestroy( ) override;

	GLuint GetVertexObject( ) const { return m_VertexObject; }

	virtual void BeginRender( class CMeshRenderer* renderer, SMeshDrawInfo& outInfo );
	virtual void EndRender( class CMeshRenderer* renderer );

private:
	GLuint		m_VertexObject = -1;
};