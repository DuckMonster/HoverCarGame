#pragma once
#include "Component.h"

class CCamera : public CComponent
{
public:
	CCamera( CActor* actor );

	void SetAspect( const float& aspect ) { m_Aspect = aspect; }
	void SetOrthoHeight( const float& height ) { m_OrthoHeight = height; }
	void SetProjection( const bool& projection ) { m_Perspective = projection; }
	void SetFieldOfView( const float& fieldOfView ) { m_FieldOfView = fieldOfView; }
	void SetNear( const float& near ) { m_Near = near; }
	void SetFar( const float& far ) { m_Far = far; }

	float GetAspect( ) const { return m_Aspect; }
	float GetOrthoHeight( ) const { return m_OrthoHeight; }
	bool GetPerspective( ) const { return m_Perspective; }
	float GetFieldOfView( ) const { return m_FieldOfView; }
	float GetNear( ) const { return m_Near; }
	float GetFar( ) const { return m_Far; }

	const glm::mat4& GetMatrix( );
	const glm::mat4& GetView( ) const { return m_ViewMatrix; }
	const glm::mat4& GetProjection( ) const { return m_ProjectionMatrix; }

private:
	glm::mat4	m_ProjectionMatrix;
	glm::mat4	m_ViewMatrix;
	glm::mat4	m_CameraMatrix;

	float	m_Aspect = -1.f;
	float	m_OrthoHeight = 5.f;
	bool	m_Perspective = false;
	float	m_FieldOfView = 90.f;

	float	m_Near = 0.5f;
	float	m_Far = 100.f;
};