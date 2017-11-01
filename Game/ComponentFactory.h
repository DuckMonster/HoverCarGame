#pragma once
#include <vector>
#include "Types.h"

class CComponent;
class CActor;

typedef CComponent*( *Component_Make_Func )( CActor* actor );
typedef CComponent*( *Component_Copy_Func )( CActor* actor, const CComponent* other );

struct FactoryFunctions
{
	Component_Make_Func make = nullptr;
	Component_Copy_Func copy = nullptr;
};

struct ComponentType
{
	const char* name = nullptr;
	component_type_id id = -1;
	FactoryFunctions funcs;

	bool operator ==( const ComponentType& other ) const
	{
		return id == other.id;
	}

	bool IsValid() const
	{
		return id != (component_type_id)-1;
	}
};

class CComponentFactory
{
	static component_type_id s_NumFactories;

	template<class T>
	struct ComponentTypeLookup
	{
	public:
		static ComponentType Type;
	};

public:
	static void RegisterAllComponents();

	template<class T>
	static ComponentType RegisterComponent( const char* name )
	{
		component_type_id index = s_NumFactories;
		s_NumFactories++;

		ComponentType Type;
		Type.name = name;
		Type.id = index;
		CreateFunctions<T>( Type.funcs );

		ComponentTypeLookup<T>::Type = Type;

		s_Factories.push_back( Type );
		Ensure( s_Factories.size() == s_NumFactories );

		Debug_Log( "%s register [%d]", typeid( T ).name(), index );

		return Type;
	}

	template<class T>
	__forceinline static ComponentType GetType()
	{
		ComponentType type = ComponentTypeLookup<T>::Type;
		Ensure( type.IsValid() );

		return type;
	}

	__forceinline static ComponentType GetType( component_type_id id )
	{
		if ( EnsureNot( id >= s_Factories.size() ) )
			return ComponentType();

		return s_Factories[id];
	}

private:
	static std::vector<ComponentType> s_Factories;

	template<class T>
	static void CreateFunctions( FactoryFunctions& funcs )
	{
		funcs.copy = &CopyFunction<T>;
		funcs.make = &MakeFunction<T>;
	}

	template<class T>
	static CComponent* MakeFunction( CActor* actor )
	{
		Ensure( actor );
		return new T( actor );
	}

	template<class T>
	static CComponent* CopyFunction( CActor* actor, const CComponent* other )
	{
		Ensure( other );
		T* comp = new T( *(T*)other );
		comp->SetActor( actor );

		return comp;

	}
};

template<class T>
ComponentType CComponentFactory::ComponentTypeLookup<T>::Type;