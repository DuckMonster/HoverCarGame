#pragma once
#include "Types.h"

namespace Network
{
	enum class GameMessage : char
	{
		Default,
		Scene,

		Channel_Close,
		Channel_Actor,
		Channel_RPC,
		Channel_AddPeers,
		Channel_RemovePeers
	};

	typedef unsigned int channel_id;
	typedef unsigned char rpc_id;
}