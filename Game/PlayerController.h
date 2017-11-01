#pragma once
#include "Component.h"
#include "Material.h"
#include "CharacterController.h"

class CPlayerController : public CCharacterController
{
	enum InputType
	{
		INPUT_Right = 1 << 0,
		INPUT_Left = 1 << 1,
		INPUT_Up = 1 << 2,
		INPUT_Down = 1 << 3
	};

	class InputByte
	{
	public:
		void SetKey( InputType type, bool value )
		{
			if ( value )
				m_Value |= (unsigned char)type;
			else
				m_Value &= ~( (unsigned char)type );
		}

		bool GetKey( InputType type )
		{
			return m_Value & (unsigned char)type;
		}

		bool operator ==( const InputByte& other ) { return m_Value == other.m_Value; }
		bool operator !=( const InputByte& other ) { return !( *this == other ); }

		unsigned char Value() { return m_Value; }

	private:
		unsigned char m_Value = 0;
	};

public:
	CPlayerController( CActor* actor );
	~CPlayerController();

	void OnLoad( const CDataStructure& data ) override;
	void OnBegin() override;
	void OnPreUpdate( const SUpdateInfo& info ) override;

	glm::vec2 GetInputVector() override;
	float GetTargetDirection() override { return std::atan2f( -m_AimPosition.y, m_AimPosition.x ); }

	const glm::vec2& GetAimPosition() const { return m_AimPosition; }

	//--------------------------------------------------- Possession
#if defined(GAME_SERVER)
	void SendPossess( neteng::peer_hash hash );
#endif
	void NetPossess();
	Network::RPC_Handle* RPC_Possess;

private:
	//--------------------------------------------------- RPCs
	void NetSetInput( InputByte& byte, glm::vec3& position, glm::vec2& aimPosition );
	Network::RPC_Handle* RPC_SetInput;

	//--------------------------------------------------- Vars
	glm::vec2 m_AimPosition;

	//--------------------------------------------------- Input
	void UpdateInput( const SUpdateInfo& info );
	InputByte m_InputByte;

	bool m_IsLocal = false;
};

