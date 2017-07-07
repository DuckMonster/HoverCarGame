#pragma once

class CActor
{
private:
	typedef std::vector<CActor*>::iterator actor_iterator;

public:
	CActor( class CScene* scene );

	void Init( );
	void Destroy( );

	void Update( float delta );
	void Render( float delta );

	void SetParent( CActor* target );

	// Spawning
	void SpawnActor( const std::string& name, CActor* parent = nullptr );

	void SetName( const std::string& name ) { m_Name = name; }
	const std::string& GetName( ) const { return m_Name; }

private:
	class CScene*	m_Scene = nullptr;
	CActor*			m_Parent = nullptr;
	std::vector<CActor*> m_Children;

	std::string		m_Name = "";
};