#pragma once

class CGame
{
public:
	CGame( );
	~CGame( );

	void Update( float delta );
	void Render( float delta );

private:
	class CScene*	m_Scene;
};