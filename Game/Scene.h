#pragma once
class CScene
{
public:
	CScene( class CGame* game );
	~CScene( );

	void Update( float delta );
	void Render( float delta );

	class CActor* GetRootActor( ) const { return m_RootActor; }
	class CGame* GetGame( ) const { return m_Game; }
	class CCamera* GetActiveCamera( ) const { return m_ActiveCamera; }
	class CCamera* SetActiveCamera( CCamera* camera ) { m_ActiveCamera = camera; }

private:
	class CGame* const	m_Game;
	class CActor*	m_RootActor = nullptr;
	class CCamera*	m_ActiveCamera = nullptr;
};