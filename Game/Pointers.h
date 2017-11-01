#pragma once
#include <set>

class IRefHolder
{
public:
	virtual void OnDeleted() = 0;
};

class CPtrType
{
public:
	void AddRef( IRefHolder* holder )
	{
		m_RefHolders.insert( holder );
	}
	void RemoveRef( IRefHolder* holder )
	{
		m_RefHolders.erase( holder );
	}

protected:
	virtual ~CPtrType() { DeletePtr(); }

private:
	void DeletePtr()
	{
		for ( IRefHolder* holder : m_RefHolders )
			holder->OnDeleted();
	}

	std::set<IRefHolder*> m_RefHolders;
};

template<typename T>
class TWeakPointer : public IRefHolder
{
public:
	TWeakPointer() {}
	TWeakPointer( T* object ) { SetObject( object ); }
	TWeakPointer( const TWeakPointer<T>& other ) { SetObject( other.m_Object ); }
	~TWeakPointer() { SetObject( nullptr ); }

	void operator =( const TWeakPointer<T>& other ) { SetObject( other.m_Object ); }
	bool operator ==( const TWeakPointer<T>& other ) const { m_Object == other.m_Object; }
	bool operator ==( const T* ptr ) const { return m_Object == ptr; }
	operator bool() const { return IsValid(); }

	T* Get() const { return (T*)m_Object; }
	T* operator ->() { return (T*)m_Object; }
	T& operator *() { return *(T*)m_Object; }

	bool IsValid() const { return m_Object != nullptr; }

protected:
	void OnDeleted() override
	{
		m_Object = nullptr;
	}

private:
	void SetObject( CPtrType* object )
	{
		if ( m_Object )
			m_Object->RemoveRef( this );

		m_Object = object;

		if ( m_Object )
			m_Object->AddRef( this );
	}

	CPtrType* m_Object = nullptr;
};