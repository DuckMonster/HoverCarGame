#include "stdafx.h"
#include "LineRenderer.h"
#include "Transform.h"
#if !defined(GAME_SERVER)

using namespace glm;

/**	On Init
*******************************************************************************/
void CLineRenderer::OnInit()
{
	CComponent::OnInit();

	//--------------------------------------------------- Init buffer and vertex object
	glGenVertexArrays( 1, &m_VertexObject );
	glGenBuffers( 1, &m_VertexBuffer );

	glBindBuffer( GL_ARRAY_BUFFER, m_VertexBuffer );

	glBindVertexArray( m_VertexObject );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, 0, 0 );
	glBindVertexArray( 0 );

	m_Material.LoadFromFile( "../Assets/Shader/Deferred/D_linerenderer" );
}

/**	On Begin
*******************************************************************************/
void CLineRenderer::OnBegin()
{
	AddSegment( Transform()->GetWorldPosition() );
}

/**	On Destroy
*******************************************************************************/
void CLineRenderer::OnDestroy()
{
	glDeleteVertexArrays( 1, &m_VertexObject );
	glDeleteBuffers( 1, &m_VertexBuffer );
}

/**	On Update
*******************************************************************************/
void CLineRenderer::OnUpdate( const SUpdateInfo & info )
{
	CComponent::OnUpdate( info );

	vec3 position = Transform()->GetWorldPosition();

	if ( length( position - m_LastPosition ) > 5.f )
	{
		AddSegment( position );
	}

	UpdateSegments( info.Delta );
}

/**	On Render
*******************************************************************************/
void CLineRenderer::OnRender( const SRenderInfo & info )
{
	CComponent::OnRender( info );

	glUseProgram( m_Material.GetProgramHandle() );
	m_Material.Set( "u_Camera", info.CameraMatrix );

	glBindVertexArray( m_VertexObject );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 2 * m_SegmentData.size() );
}

/**	Add Segment
*******************************************************************************/
void CLineRenderer::AddSegment( const glm::vec3& position )
{
	vec3 direction, tangent;

	if ( position == m_LastPosition )
	{
		direction = tangent = vec3( 1.f, 0.f, 0.f );
	}
	else
	{
		direction = normalize( position - m_LastPosition );
		tangent = normalize( cross( direction, vec3( 0.f, 1.f, 0.f ) ) );
	}

	if ( m_MetaData.size() == 0 )
	{
		MetaSegment segment;
		segment.point = position;
		segment.tangent = tangent;
		segment.time = 0.f;

		// Copy this segment
		m_MetaData.push_back( segment );
	}
	else
	{
		MetaSegment segment;
		segment.point = position;
		segment.tangent = tangent;
		segment.time = 1.f;

		// Copy this segment
		m_MetaData.push_back( segment );
	}

	m_LastPosition = position;
	m_FirstSegment = false;
}

/**	Update Segments
*******************************************************************************/
void CLineRenderer::UpdateSegments( const float delta )
{
	{
		vec3 position = Transform()->GetWorldPosition();

		MetaSegment& lastSegment = LastSegment();
		vec3 direction = normalize( position - m_LastPosition );
		vec3 tangent = normalize( cross( direction, vec3( 0.f, 1.f, 0.f ) ) );

		lastSegment.point = position;
		lastSegment.tangent = tangent;
	}

	m_SegmentData.resize( m_MetaData.size() );

	for ( size_t i = 0; i < m_SegmentData.size(); i++ )
	{
		size_t next = i + 1;

		Segment& seg = m_SegmentData[i];
		MetaSegment& metaSeg = m_MetaData[i];

		vec3 pos = metaSeg.point,
			tan = metaSeg.tangent;

		metaSeg.time = max( metaSeg.time - delta, 0.f );

		seg.verts[0] = pos + tan * metaSeg.time * m_Width;
		seg.verts[1] = pos - tan * metaSeg.time * m_Width;
	}

	glBindBuffer( GL_ARRAY_BUFFER, m_VertexBuffer );

	if ( m_MetaData.size() != m_LastSegmentCount )
	{
		glBufferData( GL_ARRAY_BUFFER, sizeof( Segment ) * m_SegmentData.size(), &m_SegmentData[0], GL_STREAM_DRAW );
		m_LastSegmentCount = m_MetaData.size();
	}
	else
	{
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( Segment ) * m_SegmentData.size(), &m_SegmentData[0] );
	}
}
#endif