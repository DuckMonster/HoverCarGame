#include "stdafx.h"
#include "CarController.h"
#include "Transform.h"

using namespace glm;
using namespace sf;

CCarController::CCarController( CActor * actor ) : CComponent( actor )
{
}

void CCarController::OnUpdate( const SUpdateData& data )
{
	float turnAngle = 0.f;
	if (data.Input.KeyDown( Keyboard::Right ))
		turnAngle -= 90.f;
	if (data.Input.KeyDown( Keyboard::Left ))
		turnAngle += 90.f;

	if (turnAngle != 0.f)
		//Transform( )->AddLocalRotationEuler( vec3( 0.f, turnAngle * data.Delta, 0.f ) );
		Transform( )->AddLocalRotation( angleAxis( turnAngle * data.Delta * Math::Deg2Rad, vec3( 0.f, 1.f, 0.f ) ) );

	if (data.Input.KeyDown(Keyboard::Up))
		Transform( )->AddLocalPosition( Transform( )->GetForward( ) * 6.f * data.Delta );
}