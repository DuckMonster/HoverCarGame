#include "stdafx.h"
#include "CameraController.h"
#include "Transform.h"

using namespace glm;

/**	Constructor
*******************************************************************************/
CCameraController::CCameraController( CActor * actor ) : CComponent( actor )
{
}

/**	On Init
*******************************************************************************/
void CCameraController::OnInit( )
{
	CComponent::OnInit();
}

/**	On Update
*******************************************************************************/
void CCameraController::OnUpdate( const SUpdateData & data )
{
	CComponent::OnUpdate(data);

	if (m_Target == nullptr)
		return;

	vec3 offset( 5.f, 5.f, 5.f );

	vec3 position = Transform( )->GetPosition( ) - offset;
	vec3 target = m_Target->GetPosition( );

	position = lerp( position, target, 5.f * data.Delta );

	Transform( )->SetPosition( position + offset );
	Transform( )->LookAt( position - offset );
}
