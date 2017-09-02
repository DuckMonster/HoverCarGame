#pragma once
#include "ActorData.h"

class CComponent
{
public:
	CComponent( class CActor* actor, const char* name );
	CComponent( const CComponent&& other ) = delete;
	virtual ~CComponent( );

	virtual void OnInit( );
	virtual void OnDestroy( );
	virtual void OnUpdate( const SUpdateInfo& info );
	virtual void OnRender( const SRenderInfo& info );

	class CActor* GetActor( ) const { return m_Actor; }
	class CTransform* Transform( ) const;

	std::string GetName( ) const { return m_Name; }

private:
	class CActor* const	m_Actor;
	const char* m_Name;
};