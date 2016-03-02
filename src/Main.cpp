#include "Engine/Engine.hpp"

#include <iostream>

namespace he = Helix;
 
int main( int argc, char* args[] )
{
    int posX = 100;
    int posY = 200;
    int sizeX = 800;
    int sizeY = 600;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    SDL_Rect playerPos;
    playerPos.x = 20;
    playerPos.y = 20;
    playerPos.w = 20;
    playerPos.h = 20;
    
    //he::Gui* something = new he::Gui();
    //something->test();
 
    if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow( "Helix", posX, posY, sizeX, sizeY, 0 );
    if ( window == nullptr )
    {
        std::cout << "Failed to create window : " << SDL_GetError();
        return -1;
    }

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if ( renderer == nullptr )
    {
        std::cout << "Failed to create renderer : " << SDL_GetError();
        return -1;
    }
    
    he::Fps fps;
    fps.Init();
    
    bool running = true;
    
    while( running )
    {
		SDL_Event e;
		
		while( SDL_PollEvent( &e ) )
		{
			if( e.type == SDL_QUIT )
			{
				running = false;
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
						running = false;
					break;

					default:
					break;
				}
			}
		}
		
		SDL_RenderSetLogicalSize( renderer, sizeX, sizeY );

		SDL_RenderClear( renderer );

		SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );

		SDL_RenderFillRect( renderer, &playerPos );

		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		
		SDL_RenderPresent( renderer );
		
		fps.Show();
		
		//SDL_Delay(16); // 500 should make 2 frames per second.
	}
    
    SDL_DestroyWindow( window );

	SDL_Quit();
}
