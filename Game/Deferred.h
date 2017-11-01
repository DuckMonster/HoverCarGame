#pragma once
#if !defined(GAME_SERVER)
#include "Scene.h"

struct GBuffer
{
	GLuint	Color;
	GLuint	Normal;
	GLuint	Depth;
};

struct SFramebufferSetup
{
	GLuint Framebuffer;
	GLuint Texture;
	GLuint Shader;
};

class CDeferred
{
public:
	CDeferred( );
	~CDeferred( );

	void RenderScene( CScene* scene );

private:
	void GBufferPass( CScene* scene );
	void ShadowPass( CScene* scene );
	void LightPass( CScene* scene );
	void ForwardPass( CScene* scene );

	void InitGBuffer( );
	void DestroyGBuffer( );

	void Recompile( );

	int		m_Width, m_Height;

	//--------------------------------------------------- GBuffer
	bool	m_GBufferValid = false;
	GBuffer m_GBuffer;
	GLuint	m_GFrameBuffer;

	SFramebufferSetup m_ShadowPass;
	SFramebufferSetup m_LightPass;
	SFramebufferSetup m_ForwardPass;

	glm::mat4 m_ViewInv;
	glm::mat4 m_ProjInv;

	glm::mat4 m_LightSpace;
};
#endif