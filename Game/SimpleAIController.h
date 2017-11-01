#pragma once
#include "CharacterController.h"

class CPlayerController;

class CSimpleAIController : public CCharacterController
{
public:
	CSimpleAIController( CActor* actor );

	void OnPreUpdate( const SUpdateInfo& info );
	void Serialize( CSerializer& serializer );

	float GetTargetDirection() override;

private:
	//--------------------------------------------------- RPCs
	void NetSetTarget( CActor* target );
	Network::RPC_Handle* RPC_SetTarget;
	//--------------------------------------------------- Comment
	
	void LookForTarget();

	float m_SearchTimer = 0.5f;
	TWeakPointer<CPlayerController> m_Target = nullptr;
};
