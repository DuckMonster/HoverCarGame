#pragma once
#include "Component.h"

class CMaterial;

class CCustomRenderer : public CComponent
{
public:
	CCustomRenderer( CActor* actor ) : CComponent( actor ) { Register<CCustomRenderer>(); }

#if defined(GAME_SERVER)
	void OnInit() override {}
	void OnDestroy() override {};

	GLuint AddBuffer() { return 0; }
	void BindAttribute( size_t bufferIndex, GLuint attrib, GLenum type, GLuint size, size_t stride, size_t offset ) {}
#else
	void OnInit() override;
	void OnDestroy() override;
	void OnRender( const SRenderInfo& info ) override;

	GLuint AddBuffer();
	void BindAttribute( size_t bufferIndex, GLuint attrib, GLenum type, GLuint size, size_t stride, size_t offset );
#endif

	void SetMaterial( CMaterial* material ) { m_Material = material; }
	CMaterial* GetMaterial() const { return m_Material; }

	void SetDrawCount( size_t count ) { m_DrawCount = count; }
	size_t GetDrawCount() const { return m_DrawCount; }
	
	void SetDrawMode( GLenum renderType ) { m_DrawMode = renderType; }
	GLenum GetDrawMode() const { return m_DrawMode; }

private:
	size_t m_DrawCount = 0;
	GLenum m_DrawMode = GL_TRIANGLES;

	CMaterial* m_Material = nullptr;
	GLuint m_VertexObject;
	std::vector<GLuint> m_Buffers;
};
