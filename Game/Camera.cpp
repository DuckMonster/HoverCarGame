#include "stdafx.h"
#include "Camera.h"
#include "Time.h"
#include "Transform.h"

using namespace glm;

/**	Constructor
*******************************************************************************/
CCamera::CCamera( CActor * actor ) : CComponent( actor, "Camera" )
{
}

/**	Get Matrix
*******************************************************************************/
const glm::mat4& CCamera::GetMatrix( )
{
	if (m_Perspective)
		m_ProjectionMatrix = perspective<float>( radians( m_FieldOfView ), m_Aspect, m_Near, m_Far );
	else
		m_ProjectionMatrix = ortho<float>( -m_OrthoHeight * m_Aspect, m_OrthoHeight * m_Aspect, -m_OrthoHeight, m_OrthoHeight, m_Near, m_Far );

	vec3 forward = Transform( )->GetForward( ),
		up = Transform( )->GetUp( ),
		offset = Transform( )->GetPosition( );

	m_ViewMatrix = lookAt( offset, offset + forward, up );

	m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	return m_CameraMatrix;
}