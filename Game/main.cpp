#include "stdafx.h"
#include <SFML/Window.hpp>
#include "Game.h"
#include "Time.h"

int main( )
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;

	sf::Window window( sf::VideoMode( 1600, 900 ), "Hover Bike Game", sf::Style::Default, settings );

	window.setFramerateLimit( 150 );
	glewInit( );

	CTime::Reset( );
	CGame game;
	game.OnViewportChange( window.getSize( ).x, window.getSize( ).y );

	while (window.isOpen( ))
	{
		sf::Event event;
		while (window.pollEvent( event ))
		{
			switch (event.type)
			{
				//--------------------------------------------------- Closed
			case sf::Event::Closed:
				window.close( );
				break;

				//--------------------------------------------------- Resized
			case sf::Event::Resized:
				game.OnViewportChange( window.getSize( ).x, window.getSize( ).y );
				break;

				//--------------------------------------------------- Key Pressed
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close( );

				game.KeyEvent( event.key.code, true );
				break;

				//--------------------------------------------------- Key Released
			case sf::Event::KeyReleased:
				game.KeyEvent( event.key.code, false );
				break;

			case sf::Event::JoystickButtonPressed:
				Print_Log( "%d pressed", event.joystickButton.button );
				game.ButtonEvent( event.joystickButton.joystickId, event.joystickButton.button, true );
				break;

			case sf::Event::JoystickButtonReleased:
				game.ButtonEvent( event.joystickButton.joystickId, event.joystickButton.button, false );
				break;

			case sf::Event::JoystickMoved:
				Print_Log( "Axis %d: %f", event.joystickMove.axis, event.joystickMove.position );
				game.AxisEvent( event.joystickMove.joystickId, event.joystickMove.axis, event.joystickMove.position / 100.f );
				break;
			}
		}

		game.OnFrame( );
		window.display( );
	}

	return 0;
}