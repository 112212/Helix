#include "Engine/Engine.hpp"

#include <SDL2/SDL.h>
#include <iostream>

namespace he = Helix;

int main(int argc, char* args[])
{
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	
	he::Gui* something = new he::Gui();
	something->test();

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Error initializing SDL! " << SDL_GetError() << std::endl;
	}
	else
	{
		window = SDL_CreateWindow("Helix demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(window == NULL)
		{
			std::cout << "Error creating window! " << SDL_GetError() << std::endl;
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
			
			bool quit = false;

			SDL_Event e;

			while(!quit)
			{
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
							case SDLK_UP:
								std::cout << "up" << std::endl;
							break;

							case SDLK_DOWN:
								std::cout << "down" << std::endl;
							break;

							case SDLK_LEFT:
								std::cout << "left" << std::endl;
							break;

							case SDLK_RIGHT:
								std::cout << "right" << std::endl;
							break;
							
							case SDLK_ESCAPE:
								quit = true;
							break;

							default:
							break;
						}
					}
				}
			
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

				SDL_UpdateWindowSurface(window);
			}
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
