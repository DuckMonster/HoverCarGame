#include "stdafx.h"
#if !defined(GAME_SERVER)
#include "Forward.h"

#include "Scene.h"
#include "GLUtils.h"
#include "Camera.h"
#include "Actor.h"

using namespace glm;

/**	Constructor
*******************************************************************************/
CForward::CForward( ) :
	m_Width( 1600 / 4 ), m_Height( 900 / 4 )
{
}

/**	Destructor
*******************************************************************************/
CForward::~CForward( )
{
	Destroy( );
}

void CForward::Render( CScene* scene, float delta )
{
	static bool PreviousSpace = false;

	bool SpaceIsPressed = sf::Keyboard::isKeyPressed( sf::Keyboard::Space );
	if (SpaceIsPressed && !PreviousSpace)
	{
		RecompileShaders( );
	}
	PreviousSpace = SpaceIsPressed;

	Create( );

	CCamera* camera = scene->GetActiveCamera( );
	SRenderInfo info;
	info.CameraMatrix = camera->GetMatrix( );

	// Get old viewport
	GLint Viewport[4];
	glGetIntegerv( GL_VIEWPORT, Viewport );

	// Bind framebuffer
	glViewport( 0, 0, m_Width, m_Height );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FrameBuffer );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Render
	scene->GetRootActor( )->Render( info );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// TESTING
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//GLUtils::RenderTexture( m_ColorTexture );
	//GLUtils::RenderTexture( m_DepthTexture );

	// Motion blur
	glClearColor( 0.05f, 0.05f, 0.05f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glUseProgram( m_MotionBlurShader );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_DepthTexture );
	/*glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, m_MotionBlurOutTexture );
	glActiveTexture( GL_TEXTURE0 );*/

	CCamera* Camera = scene->GetActiveCamera( );

	GLUtils::SetShaderUniform( m_MotionBlurShader, "u_InvProjection", inverse( Camera->GetProjection( ) ) );
	GLUtils::SetShaderUniform( m_MotionBlurShader, "u_InvView", inverse( Camera->GetView( ) ) );
	GLUtils::SetShaderUniform( m_MotionBlurShader, "u_Sampler", 0 );
	GLUtils::SetShaderUniform( m_MotionBlurShader, "u_PrevSampler", 1 );

	glBindFramebuffer( GL_FRAMEBUFFER, m_MotionBlurFrameBuffer );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDrawArrays( GL_QUADS, 0, 4 );

	// Reset
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( Viewport[0], Viewport[1], Viewport[2], Viewport[3] );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	GLUtils::RenderTexture( m_ColorTexture );
	//GLUtils::RenderTexture( m_DepthTexture );
	//m_FirstTexture = !m_FirstTexture;
}

/**	Create
*******************************************************************************/
void CForward::Create( )
{
	if (m_Valid)
		return;

	//--------------------------------------------------- Frame Buffer
	glCreateFramebuffers( 1, &m_FrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FrameBuffer );

	// Init textures and stuff
	glGenTextures( 1, &m_ColorTexture );
	glGenTextures( 1, &m_DepthTexture );

	glBindTexture( GL_TEXTURE_2D, m_ColorTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0 );

	glBindTexture( GL_TEXTURE_2D, m_DepthTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0 );

	if (glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE)
		Debug_Log( "Forward Framebuffer Not Complete" );
	else
		Debug_Log( "Forward Framebuffer Created!" );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	//--------------------------------------------------- Motion blur
	m_MotionBlurShader = GLUtils::CreateShaderFromFile( "../Assets/Shader/motion_blur" );
	glGenTextures( 1, &m_MotionBlurOutTexture );

	glBindTexture( GL_TEXTURE_2D, m_MotionBlurOutTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glGenFramebuffers( 1, &m_MotionBlurFrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, m_MotionBlurFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_MotionBlurOutTexture, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	m_Valid = true;
}

/**	Destroy
*******************************************************************************/
void CForward::Destroy( )
{
	if (!m_Valid)
		return;

	glDeleteFramebuffers( 1, &m_FrameBuffer );

	m_Valid = false;
}

void CForward::RecompileShaders( )
{
	glDeleteProgram( m_MotionBlurShader );
	m_MotionBlurShader = GLUtils::CreateShaderFromFile( "../Assets/Shader/motion_blur" );
}
#endif
