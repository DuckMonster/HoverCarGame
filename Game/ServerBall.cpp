#include "stdafx.h"
#include "ServerBall.h"
#include "Transform.h"
#include "Network.h"
#include "Random.h"

using namespace glm;
using namespace Network;

CServerBall::CServerBall( CActor* actor ) : CComponent( actor )
{
	Register<CServerBall>();
	RPC_Jump = RegisterRPC( &CServerBall::JumpNet, RPC_Policy::BroadcastAll() );
}

void CServerBall::OnBegin()
{
}

void CServerBall::OnUpdate( const SUpdateInfo & info )
{
	const static float GRAVITY = -20.f;
	m_Velocity += GRAVITY * info.Delta;

	vec3 newPosition = Transform()->GetPosition();

	// Bounce
	if ( newPosition.y + m_Velocity * info.Delta <= Transform()->GetScale().x / 2.f )
	{
		m_Velocity = abs( m_Velocity * 0.8f );
	}

	newPosition.y += m_Velocity * info.Delta;
	Transform()->SetPosition( newPosition );

	// Stuck in ground....
	if ( Transform()->GetPosition().y <= Transform()->GetScale().x / 2.f )
	{
		vec3 newPos = Transform()->GetPosition();
		newPos.y = 5.f;

		Transform()->SetPosition( newPos );
		m_Velocity = 0.f;
	}

	if ( Network::IsServer() )
	{
		m_JumpTimer -= info.Delta;
		if ( m_JumpTimer <= 0.f )
		{
			RPC_Jump->Send( CRandom::Value( 0.5f, 1.5f ) );
			m_JumpTimer += 1.f / m_JumpFrequency;
		}
	}
}

void CServerBall::Serialize( CSerializer& serializer )
{
	CComponent::Serialize( serializer );

	serializer.Serialize( m_JumpFrequency );
	serializer.Serialize( m_JumpStrength );
}

void CServerBall::Jump( float scale )
{
	m_Velocity = m_JumpStrength * scale;
}
