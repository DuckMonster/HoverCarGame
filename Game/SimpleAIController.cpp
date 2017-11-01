#include "stdafx.h"
#include "SimpleAIController.h"
#include "PlayerController.h"
#include "Scene.h"
#include "Actor.h"
#include "Random.h"
#include "Transform.h"

using namespace glm;
using namespace Network;

/**	Constructor
*******************************************************************************/
CSimpleAIController::CSimpleAIController( CActor * actor ) : CCharacterController( actor )
{
	Register<CSimpleAIController>();

	RPC_SetTarget = RegisterRPC( &CSimpleAIController::NetSetTarget, RPC_Policy::BroadcastPeers() );
}

/**	On Pre Update
*******************************************************************************/
void CSimpleAIController::OnPreUpdate( const SUpdateInfo & info )
{
#if defined(GAME_SERVER)
	if ( !m_Target )
	{
		m_SearchTimer -= info.Delta;

		if ( m_SearchTimer <= 0.f )
		{
			LookForTarget();
			m_SearchTimer = CRandom::Value( 0.3f, 0.6f );
		}
	}
#endif
}

/**	Serialize
*******************************************************************************/
void CSimpleAIController::Serialize( CSerializer & serializer )
{
	if ( serializer.IsWriting() )
	{
		CActor* actor = nullptr;

		if ( m_Target )
			actor = m_Target->GetActor();

		serializer.Serialize( actor );
	}
	else
	{
		CActor* actor = nullptr;
		serializer.Serialize( actor );

		if ( actor )
			m_Target = actor->GetComponent<CPlayerController>();
	}
}

/**	Get Target Direction
*******************************************************************************/
float CSimpleAIController::GetTargetDirection()
{
	if ( !m_Target )
		return 0.f;

	vec2 dirToPlayer = Math::Make2D( m_Target->Transform()->GetPosition() - Transform()->GetPosition() );
	return std::atan2f( -dirToPlayer.y, dirToPlayer.x );
}

/**	Net Set Target
*******************************************************************************/
void CSimpleAIController::NetSetTarget( CActor* target )
{
	Debug_Log( "Received Target \"%s\"", target->GetName().c_str() );
	m_Target = target->GetComponent<CPlayerController>();
}

/**	Look For Target
*******************************************************************************/
void CSimpleAIController::LookForTarget()
{
	if ( m_Target )
		return;

	m_Target = GetScene()->GetRootActor()->FindComponent<CPlayerController>();

	if ( m_Target )
		RPC_SetTarget->Send( m_Target->GetActor() );
}