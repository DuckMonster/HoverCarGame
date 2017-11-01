#pragma once
#include "Component.h"
#include "StringUtils.h"

class CServerBall : public CComponent
{
public:
	CServerBall( CActor* actor );

	void OnBegin() override;
	void OnUpdate( const SUpdateInfo& info ) override;
	void Serialize( CSerializer& serializer ) override;

	void Set( float freq, float str ) { m_JumpFrequency = freq; m_JumpStrength = str; }

	std::string GetDisplayString() const override { return StringUtils::Printf( "{ Freq: %.1f, Str: %.1f }", m_JumpFrequency, m_JumpStrength ); }

private:
	Network::RPC_Handle *RPC_Jump;

	void Jump( float scalar );
	void JumpNet( float scale )
	{
		Jump( scale );
	}

	float m_Velocity = 0.f;

	float m_JumpTimer = 0.f;

	float m_JumpFrequency = 0.2f;
	float m_JumpStrength = 10.f;
};