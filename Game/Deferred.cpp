#include "stdafx.h"
#include "Deferred.h"
#include "GLUtils.h"
#include "Actor.h"
#include "DirectionalLight.h"
#include "Camera.h"
#include "Game.h"
#include "Transform.h"

namespace
{
	const GLenum DRAW_BUFFERS_STANDARD[] ={ GL_COLOR_ATTACHMENT0 };
	const GLenum DRAW_BUFFERS_DEFERRED[] ={ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	const glm::vec3 LIGHT_POSITION( 20.f, 10.f, 5.f );
	const glm::vec3 LIGHT_DIRECTION = normalize( -LIGHT_POSITION );
}

/**	Constructor
*******************************************************************************/
CDeferred::CDeferred( ) :
	m_Width( 1600 / 3 ), m_Height( 900 / 3 )
{
}

/**	Destructor
*******************************************************************************/
CDeferred::~CDeferred( )
{
	DestroyGBuffer( );
}

/**	Render Scenes
*******************************************************************************/
void CDeferred::RenderScene( CScene* scene )
{
	static int shownBuffer = 0;

	InitGBuffer( );

	const Input::CInput& input = scene->GetGame( )->GetInput( );

	if (input.KeyPressed( sf::Keyboard::Space ))
		Recompile( );

	for (int i=0; i < 4; i++)
		if (input.KeyPressed( (sf::Keyboard::Key)((int)sf::Keyboard::Num1 + i) ))
			shownBuffer = i;

	// Get old viewport
	GLint Viewport[4];
	glGetIntegerv( GL_VIEWPORT, Viewport );


	GBufferPass( scene );
	ShadowPass( scene );
	LightPass( scene );

	//--------------------------------------------------- Reset
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( Viewport[0], Viewport[1], Viewport[2], Viewport[3] );

	GLuint renderTexture = 0;

	switch (shownBuffer)
	{
	case 0: renderTexture = m_LightPass.Texture; break;
	case 1: renderTexture = m_ShadowPass.Texture; break;
	case 2: renderTexture = m_GBuffer.Color; break;
	case 3: renderTexture = m_GBuffer.Normal; break;
	}

	GLUtils::RenderTexture( renderTexture );
}

/**	GBuffer Pass
*******************************************************************************/
void CDeferred::GBufferPass( CScene * scene )
{
	CCamera* camera = scene->GetActiveCamera( );
	float aspect = (float)m_Width / m_Height;
	camera->SetAspect( aspect );

	SRenderInfo info;
	info.CameraMatrix = camera->GetMatrix( );

	m_ViewInv = inverse( camera->GetView( ) );
	m_ProjInv = inverse( camera->GetProjection( ) );

	glBindFramebuffer( GL_FRAMEBUFFER, m_GFrameBuffer );
	glDrawBuffers( 2, DRAW_BUFFERS_DEFERRED );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );
	glViewport( 0, 0, m_Width, m_Height );

	scene->GetRootActor( )->Render( info );

	glDrawBuffers( 1, DRAW_BUFFERS_STANDARD );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

/**	Shadow Pass
*******************************************************************************/
void CDeferred::ShadowPass( CScene* scene )
{
	using namespace glm;

	CDirectionalLight* light = scene->GetRootActor( )->FindComponent<CDirectionalLight>( );
	if (!light)
	{
		Print_Log( "No directional light in scene" );
		return;
	}

	vec3 position = light->Transform( )->GetPosition( ),
		forward = light->Transform( )->GetForward( ),
		up = light->Transform( )->GetUp( );

	mat4 view = lookAt( position, position + forward, up ),
		proj = ortho( -10.f, 10.f, -10.f, 10.f, -10.f, 20.f );

	m_LightSpace = proj * view;

	SRenderInfo info;
	info.CameraMatrix = proj * view;
	info.UseShaderOverride = true;
	info.ShaderOverride = m_ShadowPass.Shader;

	glBindFramebuffer( GL_FRAMEBUFFER, m_ShadowPass.Framebuffer );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );
	glViewport( 0, 0, 2048, 2048 );

	scene->GetRootActor( )->Render( info );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

/**	Light Pass
*******************************************************************************/
void CDeferred::LightPass( CScene* scene )
{
	CDirectionalLight* light = scene->GetRootActor( )->FindComponent<CDirectionalLight>( );
	if (!light)
	{
		Print_Log( "No directional light in scene" );
		return;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, m_LightPass.Framebuffer );
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST );
	glUseProgram( m_LightPass.Shader );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_GBuffer.Color );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, m_GBuffer.Normal );
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, m_GBuffer.Depth );
	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_2D, m_ShadowPass.Texture );
	glActiveTexture( GL_TEXTURE0 );

	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_LightDirection", light->Transform( )->GetForward( ) );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_ProjectionInv", m_ProjInv );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_ViewInv", m_ViewInv );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_LightSpace", m_LightSpace );

	glViewport( 0, 0, m_Width, m_Height );
	glDrawArrays( GL_QUADS, 0, 4 );
}

/**	Init GBuffer
*******************************************************************************/
void CDeferred::InitGBuffer( )
{
	if (m_GBufferValid)
		return;

	// Init textures
	glGenTextures( 3, (GLuint*)&m_GBuffer );
	for (int i=0; i < 3; i++)
	{
		glBindTexture( GL_TEXTURE_2D, ((GLuint*)&m_GBuffer)[i] );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}

	// Set up framebuffer
	glCreateFramebuffers( 1, &m_GFrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, m_GFrameBuffer );

	// Color
	glBindTexture( GL_TEXTURE_2D, m_GBuffer.Color );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GBuffer.Color, 0 );

	// Normal
	glBindTexture( GL_TEXTURE_2D, m_GBuffer.Normal );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GBuffer.Normal, 0 );

	// Depth
	glBindTexture( GL_TEXTURE_2D, m_GBuffer.Depth );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_GBuffer.Depth, 0 );

	if (glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE)
		Print_Log( "Deferred framebuffer not complete!" );
	else
		Print_Log( "GBuffer initialized successfully!" );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	//--------------------------------------------------- Shadow Pass
	glGenTextures( 1, &m_ShadowPass.Texture );
	glGenFramebuffers( 1, &m_ShadowPass.Framebuffer );
	m_ShadowPass.Shader = GLUtils::CreateShaderFromFile( "../Assets/Shader/Deferred/D_shadowpass" );

	// Setup framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, m_ShadowPass.Framebuffer );

	glBindTexture( GL_TEXTURE_2D, m_ShadowPass.Texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowPass.Texture, 0 );

	if (glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE)
		Print_Log( "Deferred Shadow Pass not complete!" );
	else
		Print_Log( "Deferred Shadow Pass initialized successfully!" );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	//--------------------------------------------------- Lighting Pass
	glGenTextures( 1, &m_LightPass.Texture );
	glGenFramebuffers( 1, &m_LightPass.Framebuffer );
	m_LightPass.Shader = GLUtils::CreateShaderFromFile( "../Assets/Shader/Deferred/D_lightpass" );

	// Setup deferred uniforms
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Color", 0 );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Normal", 1 );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Depth", 2 );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Shadow", 3 );

	// Setup framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, m_LightPass.Framebuffer );

	glBindTexture( GL_TEXTURE_2D, m_LightPass.Texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_LightPass.Texture, 0 );

	if (glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE)
		Print_Log( "Deferred Light Pass not complete!" );
	else
		Print_Log( "Deferred Light Pass initialized successfully!" );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	m_GBufferValid = true;
}

/**	Destroy GBuffer
*******************************************************************************/
void CDeferred::DestroyGBuffer( )
{
	glDeleteTextures( 3, (GLuint*)&m_GBuffer );
	glDeleteFramebuffers( 1, &m_GFrameBuffer );

	m_GBufferValid = false;
}

/**	Recompile
*******************************************************************************/
void CDeferred::Recompile( )
{
	glDeleteProgram( m_LightPass.Shader );
	glDeleteProgram( m_ShadowPass.Shader );

	m_LightPass.Shader = GLUtils::CreateShaderFromFile( "../Assets/Shader/Deferred/D_lightpass" );
	m_ShadowPass.Shader = GLUtils::CreateShaderFromFile( "../Assets/Shader/Deferred/D_shadowpass" );

	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Color", 0 );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Normal", 1 );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Depth", 2 );
	GLUtils::SetShaderUniform( m_LightPass.Shader, "u_Deferred.Shadow", 3 );
}
