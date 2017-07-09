#pragma once
#include "ActorData.h"

class CComponent
{
public:
	CComponent( class CActor* actor );
	CComponent( const CComponent&& other ) = delete;
	virtual ~CComponent( );

	virtual void OnInit( );
	virtual void OnDestroy( );
	virtual void OnUpdate( const SUpdateData& data );
	virtual void OnRender( const SRenderData& data );

	class CActor* GetActor( ) const { return m_Actor; }
	class CTransform* Transform( ) const;

private:
	class CActor* const	m_Actor;
};