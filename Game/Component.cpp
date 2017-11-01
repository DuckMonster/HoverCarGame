#include "stdafx.h"
#include "Component.h"
#include "Actor.h"
#include "ActorChannel.h"
#include "Game.h"

using namespace Network;

/**	Constructor
*******************************************************************************/
CComponent::CComponent( CActor * actor ) :
	m_Actor( actor )
{
}

/**	Copy Constructor
*******************************************************************************/
CComponent::CComponent( const CComponent & other ) :
	m_Actor( nullptr ), m_TypeStack( other.m_TypeStack )
{
	Debug_Log( "%s copied from %s::%s",
		GetName(),
		other.GetActor()->GetName(), other.GetName() );
}

/**	Destructor
*******************************************************************************/
CComponent::~CComponent()
{
	for ( RPC_Handle* handle : m_RPCList )
		delete handle;

	m_RPCList.clear();
}

/**	Copy
*******************************************************************************/
CComponent* CComponent::Copy( CActor* actor ) const
{
	return GetType().funcs.copy( actor, this );
}

/**	On Init
*******************************************************************************/
void CComponent::OnInit()
{
}

/**	On Load
*******************************************************************************/
void CComponent::OnLoad( const CDataStructure & data )
{
}

/**	On Begin
*******************************************************************************/
void CComponent::OnBegin()
{
}

/**	On Destroy
*******************************************************************************/
void CComponent::OnDestroy()
{
}

/**	Update Internal
*******************************************************************************/
void CComponent::UpdateInternal( const SUpdateInfo & info )
{
	if ( !m_HasBegun )
	{
		OnBegin();
		m_HasBegun = true;
	}

	switch ( info.Phase )
	{
	case UpdatePhase::Pre: OnPreUpdate( info ); break;
	case UpdatePhase::Main: OnUpdate( info ); break;
	case UpdatePhase::Post: OnPostUpdate( info ); break;
	}
}

CTransform* CComponent::Transform() const { return m_Actor->Transform(); }
CScene* CComponent::GetScene() const { return m_Actor->Scene(); }
CGame* CComponent::GetGame() const { return m_Actor->Game(); }
Network::CActorChannel* CComponent::GetChannel() { return m_Actor->GetChannel(); }

/**	Serialize
*******************************************************************************/
void CComponent::Serialize( CSerializer & serializer )
{
}

/**	Receive RPC
*******************************************************************************/
void CComponent::ReceiveRPC( const RPC_Source& rpc, const neteng::Packet& packet )
{
	// Look up function pointer
	if ( EnsureNotMsg( rpc.rpc >= m_RPCList.size(), "Received unknown RPC %d on component %s::%s",
		rpc.rpc, GetActor()->GetName(), GetName() ) )
		return;

	RPC_Handle* handle = m_RPCList[rpc.rpc];

	// Call member function
	handle->Recv( packet );
}