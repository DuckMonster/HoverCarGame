#include "stdafx.h"
#include <SFML/Window.hpp>
#include "Game.h"
#include "ShaderUtils.h"

const char* VERT = "\
#version 330 core\n\
layout(location = 0) in vec3 a_Vertex;\
\
uniform mat4 u_Projection;\
uniform mat4 u_Camera;\
\
void main() {\
	gl_Position = u_Projection * u_Camera * vec4(a_Vertex, 1.0);\
}\
";

const char* FRAG = "\
#version 330 core\n\
out vec4 o_Color;\
void main() {\
	o_Color = vec4(1.0, 0.0, 0.0, 1.0);\
}\
";

using namespace std::chrono;
typedef high_resolution_clock hr_clock;
typedef high_resolution_clock::time_point hr_time;

int main( )
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;

	sf::Window window( sf::VideoMode( 1024, 768 ), "Hover Bike Game", sf::Style::Default, settings );

	glewInit( );

	// Setup shader
	GLuint prog = ShaderUtils::CreateShaderProgram( VERT, FRAG );
	glUseProgram( prog );

	std::cout << "Linking program..\n";
	{
		char buffer[512];
		glGetProgramInfoLog( prog, 512, nullptr, buffer );
		std::cout << "Result:\n" << buffer << "\n";
	}

	// Uniforms
	GLuint u_Projection = glGetUniformLocation( prog, "u_Projection" ),
		u_Camera = glGetUniformLocation( prog, "u_Camera" );

	// Setup mesh
	const float meshData[] ={
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		0.f, 1.f, 0.f
	};

	GLuint meshVBO;
	glGenBuffers( 1, &meshVBO );
	glBindBuffer( GL_ARRAY_BUFFER, meshVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( meshData ), meshData, GL_STATIC_DRAW );

	// Bind to attribute
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, 0, 0 );

	// Setup matrices
	float aspect = 1024.f / 768.f;
	glm::mat4 matProjection = glm::ortho( -5.f * aspect, 5.f * aspect, -5.f, 5.f, -10.f, 20.f );
	glUniformMatrix4fv( u_Projection, 1, false, glm::value_ptr( matProjection ) );

	hr_time prevTime = hr_clock::now( );

	CGame game;

	while (window.isOpen( ))
	{
		sf::Event event;
		while (window.pollEvent( event ))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close( );
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close( );

				break;
			}
		}

		//--------------------------------------------------- Calculate Camera
		hr_time timeNow = hr_clock::now( );
		float deltaTime = duration_cast<microseconds>(timeNow - prevTime).count( ) * 1.0e-6;
		static float totalTime = 0.f;
		totalTime += deltaTime;
		prevTime = timeNow;

		glm::mat4 matCamera = glm::lookAt(
			glm::vec3( glm::sin( totalTime ), glm::cos( totalTime * 0.4231f ), glm::cos( totalTime ) ) * 5.f,
			glm::vec3( ),
			glm::vec3( 0.f, 1.f, 0.f )
		);

		glClearColor( 0.2f, 0.2f, 0.2f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUniformMatrix4fv( u_Camera, 1, false, glm::value_ptr( matCamera ) );
		glDrawArrays( GL_TRIANGLES, 0, 3 );

		game.Update( deltaTime );
		game.Render( deltaTime );

		window.display( );
	}

	return 0;
}