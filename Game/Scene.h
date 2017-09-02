#pragma once
class CScene
{
public:
	CScene( class CGame* game );
	~CScene( );

	void Update( float delta );

	class CActor* GetRootActor( ) const { return m_RootActor; }
	class CGame* GetGame( ) const { return m_Game; }
	class CCamera* GetActiveCamera( );
	class CCamera* SetActiveCamera( CCamera* camera ) { m_ActiveCamera = camera; }

	void PrintScene( );

private:
	void GetActorString( CActor* actor, int indent, std::string& outString );

	class CGame* const	m_Game;
	class CActor*	m_RootActor = nullptr;
	class CCamera*	m_ActiveCamera = nullptr;
};