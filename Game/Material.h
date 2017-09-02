#pragma once
class CMaterial
{
public:
	CMaterial( );
	~CMaterial( );

	void Unload( );

	void LoadSource( const char* vertSrc, const char* fragSrc );

	void LoadFromFile( const char* path );
	void LoadFromFile( const char* vertPath, const char* fragPath );

	void LoadTexture( const char* path );
	GLuint GetTexture( ) const { return m_Texture; }
	bool HasTexture( ) const { return m_Texture != -1; }

	GLuint GetProgramHandle( ) const { return m_Program; }

private:
	GLuint	m_Program = -1;
	GLuint	m_Texture = -1;
};