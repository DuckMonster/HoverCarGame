#pragma once
class CForward
{
public:
	CForward( );
	~CForward( );

	void Render( class CScene* scene, float delta );

private:
	void Create( );
	void Destroy( );

	void RecompileShaders( );

	GLuint	m_FrameBuffer;
	GLuint	m_ColorTexture;
	GLuint	m_DepthTexture;

	bool	m_Valid = false;
	int		m_Width, m_Height;

	//--------------------------------------------------- Motion Blur
	GLuint	m_MotionBlurShader;
	GLuint	m_MotionBlurOutTexture;
	GLuint	m_MotionBlurFrameBuffer;
};