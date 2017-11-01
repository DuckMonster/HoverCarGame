#include "stdafx.h"
#include "Serialize.h"
#include "Scene.h"
#include "Actor.h"
#include "Component.h"

/**	Serialize Actor Pointer
*******************************************************************************/
void CSerializer::Serialize_Impl( CActor *& value )
{
	if ( IsWriting() )
	{
		// Set pointer-flag to correct value
		if ( value )
		{
			Serialize<unsigned char>( 1 );
			Serialize_Impl( value->GetPath().ToURI() );
		}
		else
		{
			Serialize<unsigned char>( 0 );
		}
	}
	else
	{
		value = nullptr;

		if ( !Ensure( m_Scene ) )
			return;

		// First read pointer flag
		unsigned char flag;
		Serialize<unsigned char>( flag );

		// This is a null-ptr
		if ( flag == 0 )
			return;

		CPath path;
		Serialize_Impl( path );

		CActor* actor = m_Scene->EvaluatePath( path );
		Ensure( actor );

		value = actor;
	}
}

/**	Serialize Component Pointer
*******************************************************************************/
void CSerializer::Serialize_Impl( CComponent *& value )
{
	if ( IsWriting() )
	{
		CPath path = value->GetActor()->GetPath();
		Serialize( path );
		Serialize( value->GetType().id );
	}
	else
	{
		value = nullptr;

		if ( !Ensure( m_Scene ) )
			return;

		CPath path;
		component_type_id id = -1;

		Serialize_Impl( path );

		CActor* actor = m_Scene->EvaluatePath( path );
		if ( EnsureNot( actor ) )
			return;

		value = actor->GetComponent( id );
	}
}
