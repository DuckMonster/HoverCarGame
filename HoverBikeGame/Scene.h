#pragma once
class CScene
{
public:
	CScene( class CGame* game );
	~CScene( );

	void Update( float delta );
	void Render( float delta );

	class CActor* GetRootActor( ) const { return m_RootActor; }

private:
	class CGame*	m_Game;
	class CActor*	m_RootActor;
};