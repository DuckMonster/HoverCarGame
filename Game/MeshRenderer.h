#pragma once
#include "Component.h"

class CMeshRenderer : public CComponent
{
private:
	enum EDataBuffer
	{
		Position = 0,
		Normal,
		TexCoord,
		Element
	};

	static const size_t BUFFER_COUNT = 4;

public:
	CMeshRenderer( class CActor* actor );

	void OnInit( ) override;
	void OnDestroy( ) override;
	void OnRender( const SRenderData& data ) override;

	void LoadMeshFromFile( const char* path );
	void LoadTextureFromFile( const char* path );

private:
	GLuint	m_Shader_PN = -1;
	GLuint	m_Shader_PNT = -1;

	GLuint	m_VertexObject = -1;
	GLuint	m_DataBuffers[BUFFER_COUNT];

	GLuint	m_Texture = -1;

	size_t	m_DrawCount = 0;
	bool	m_UseElementBuffer = false;
	bool	m_UseTexture = false;
};