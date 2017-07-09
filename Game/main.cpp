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
			case sf::Event::Closed:
				window.close( );
				break;

			case sf::Event::Resized:
				game.OnViewportChange( window.getSize( ).x, window.getSize( ).y );
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close( );

				game.KeyEvent( event.key.code, true );
				break;

			case sf::Event::KeyReleased:
				game.KeyEvent( event.key.code, false );
				break;
			}
		}

		game.OnFrame( );
		window.display( );
	}

	return 0;
}