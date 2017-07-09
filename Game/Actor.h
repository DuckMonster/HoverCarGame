#pragma once
#include "Input.h"
#include "ActorData.h"

class CActor
{
private:
	typedef std::vector<CActor*>::iterator actor_iterator;
	typedef std::vector<class CComponent*>::iterator component_iterator;

public:
	CActor( class CScene* scene );

	void Init( );
	void Destroy( );

	void Update( const SUpdateData& data );
	void Render( const SRenderData& data );

	// Tree helping functions
	size_t NumChildren( );
	CActor* GetParent( );
	CActor* GetChild( size_t index );

	// Spawning
	CActor* SpawnActor( const std::string& name = "", CActor* parent = nullptr );

	void SetName( const std::string& name ) { m_Name = name; }
	const std::string& GetName( ) const { return m_Name; }

	// Components
	template<typename TComp>
	TComp* AddComponent( );

	template<typename TComp>
	TComp* GetComponent( );

	template<typename TComp>
	TComp* FindComponent( );

	template<typename TComp>
	bool RemoveComponent( );
	bool RemoveComponent( class CComponent* component );

	class CTransform* Transform( ) const { return m_Transform; }

	size_t GetComponentCount( ) const { return m_Components.size( ); }

private:
	class CScene* const	m_Scene;
	class CGame* const	m_Game;

	// Transform
	class CTransform* m_Transform;

	// Components
	std::vector<class CComponent*> m_Components;
	
	std::string m_Name = "";
};

//--------------------------------------------------- Inlined Functions
/**	AddComponent
*******************************************************************************/
template<typename TComp>
inline TComp* CActor::AddComponent( )
{
	TComp* newComponent = new TComp( this );
	m_Components.push_back( newComponent );

	newComponent->OnInit( );

	return newComponent;
}

/**	Get Component
*******************************************************************************/
template<typename TComp>
inline TComp* CActor::GetComponent( )
{
	for (size_t i = 0; i < m_Components.size( ); i++)
	{
		// Type-check cast
		TComp* comp = dynamic_cast<TComp*>(m_Components[i]);

		if (comp)
			return comp;
	}

	return nullptr;
}

/**	Find Component
*******************************************************************************/
template<typename TComp>
inline TComp* CActor::FindComponent( )
{
	TComp* component = GetComponent<TComp>( );
	if (component)
		return component;

	// Search children
	for (size_t i = 0; i < NumChildren( ); i++)
	{
		component = GetChild( i )->FindComponent<TComp>( );
		if (component)
			return component;
	}

	// No component found
	return nullptr;
}

/**	Remove Component
*******************************************************************************/
template<typename TComp>
inline bool CActor::RemoveComponent( )
{
	for (size_t i = 0; i < m_Components.size( ); i++)
	{
		// Type-check cast
		TComp* comp = dynamic_cast<TComp*>(m_Components[i]);

		if (comp)
		{
			comp->OnDestroy( );
			delete comp;

			m_Components.erase( m_Components.begin( ) + i );
			return true;
		}
	}

	return false;
}
