#include "stdafx.h"
#include "Game.h"
#include "Time.h"
#include "DataStructure.h"
#include "StringUtils.h"
#include "Path.h"

int MainFunc();

int main()
{
	return MainFunc();
}

#if !defined(GAME_SERVER)
#include <SFML/Window.hpp>
int MainFunc()
{
	//--------------------------------------------------- Get client size
	int width = 1600, height = 900;

	{
		CDataStructure clientData( "../Assets/Data/init.json" );
		clientData.Read( "/Client/Width", width );
		clientData.Read( "/Client/Height", height );
	}
	//

	//--------------------------------------------------- Create window and init opengl
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	sf::Window window( sf::VideoMode( width, height ), "Hover Bike Game", sf::Style::Default, settings );

	window.setFramerateLimit( 150 );
	glewInit();

	CTime::Reset();
	CGame game;
	game.OnViewportChange( window.getSize().x, window.getSize().y );

	//--------------------------------------------------- Main loop!
	bool gameRunning = true;

	while ( window.isOpen() && gameRunning )
	{
		sf::Event event;
		while ( window.pollEvent( event ) )
		{
			switch ( event.type )
			{
				//--------------------------------------------------- Closed
			case sf::Event::Closed:
				window.close();
				gameRunning = false;
				break;

				//--------------------------------------------------- Resized
			case sf::Event::Resized:
				game.OnViewportChange( window.getSize().x, window.getSize().y );
				break;

				//--------------------------------------------------- Keyboard
			case sf::Event::KeyPressed:
				if ( event.key.code == sf::Keyboard::Escape )
					gameRunning = false;

				game.KeyEvent( event.key.code, true );
				break;

			case sf::Event::KeyReleased:
				game.KeyEvent( event.key.code, false );
				break;

				//--------------------------------------------------- Mouse
			case sf::Event::MouseMoved:
			{
				sf::Vector2i mouse = sf::Mouse::getPosition( window );
				game.MouseMoveEvent( mouse.x, mouse.y );
				break;
			}

			case sf::Event::MouseButtonPressed:
				game.MouseButtonEvent( event.mouseButton.button, true );
				break;

			case sf::Event::MouseButtonReleased:
				game.MouseButtonEvent( event.mouseButton.button, false );
				break;

				//--------------------------------------------------- Joystick
			case sf::Event::JoystickButtonPressed:
				Debug_Log( "%d pressed", event.joystickButton.button );
				game.ButtonEvent( event.joystickButton.joystickId, event.joystickButton.button, true );
				break;

			case sf::Event::JoystickButtonReleased:
				game.ButtonEvent( event.joystickButton.joystickId, event.joystickButton.button, false );
				break;

			case sf::Event::JoystickMoved:
				Debug_Log( "Axis %d: %f", event.joystickMove.axis, event.joystickMove.position );
				game.AxisEvent( event.joystickMove.joystickId, event.joystickMove.axis, event.joystickMove.position / 100.f );
				break;
			}
		}

		if ( !gameRunning )
			break;

		gameRunning = game.OnFrame();
		window.display();
	}

	return 0;
}
#else
int MainFunc()
{
	using namespace std;

	CTime::Reset();
	CGame game;

	bool gameRunning = true;

	while ( gameRunning )
	{
		gameRunning = game.OnFrame();
		this_thread::sleep_for( chrono::milliseconds( 2 ) );
	}

	return 0;
}
#endif