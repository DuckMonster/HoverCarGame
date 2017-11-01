#include "stdafx.h"
#include "Camera.h"
#include "Time.h"
#include "Transform.h"

using namespace glm;

/**	DeProject
*******************************************************************************/
Math::Ray CCamera::ScreenToWorld( const vec2& ndc )
{
	mat4 invMatrix = inverse( GetMatrix() );

	vec4 worldNear = invMatrix * vec4( ndc, -1.f, 1.f );
	vec4 worldFar = invMatrix * vec4( ndc, 1.f, 1.f );
	worldNear /= worldNear.w;
	worldFar /= worldFar.w;

	return Math::Ray::FromTo( vec3( worldNear ), vec3( worldFar ) );
}

/**	Project
*******************************************************************************/
vec2 CCamera::WorldToScreen( const vec3 & world )
{
	vec4 ndc = GetMatrix() * vec4( world, 1.0f );
	return vec2( ndc.x, ndc.y );
}

/**	Get Matrix
*******************************************************************************/
const mat4& CCamera::GetMatrix()
{
	if ( m_Perspective )
		m_ProjectionMatrix = perspective<float>( radians( m_FieldOfView ), m_Aspect, m_Near, m_Far );
	else
		m_ProjectionMatrix = ortho<float>( -m_OrthoHeight * m_Aspect, m_OrthoHeight * m_Aspect, -m_OrthoHeight, m_OrthoHeight, m_Near, m_Far );

	vec3 forward = Transform()->GetForward(),
		up = Transform()->GetUp(),
		offset = Transform()->GetWorldPosition();

	m_ViewMatrix = lookAt( offset, offset + forward, up );

	m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	return m_CameraMatrix;
}