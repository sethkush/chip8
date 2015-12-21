#include <SDL2/SDL.h>
#include <stdio.h>


int main(int argc, char* argv[]) {


  SDL_Window* window;
  SDL_Surface* screen;


  SDL_Init(SDL_INIT_VIDEO);

  //Main loop flag
  int quit = 0;

  //Event handler
  SDL_Event e;

  window = SDL_CreateWindow(
	  "CHIP 8 Emulator",                 // window title
	  SDL_WINDOWPOS_UNDEFINED,           // initial x position
	  SDL_WINDOWPOS_UNDEFINED,           // initial y position
	  640,                               // width, in pixels
	  320,                               // height, in pixels
	  SDL_WINDOW_OPENGL                  // flags - see below
	  );

  if (window == NULL) {
    // In the case that the window could not be made...
    printf("Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  // Set the surface on which we will render the screen
  screen = SDL_GetWindowSurface( window );
  
  // Fill it with black
  SDL_FillRect(
	       screen,
	       NULL,
	       SDL_MapRGB( screen->format, 0, 0, 0 )
	       );
  
  
  while( !quit ) {
    // Update the screen
    SDL_UpdateWindowSurface( window );
    while( SDL_PollEvent( &e ) != 0 ) {
      //User requests quit
      if( e.type == SDL_QUIT ) {
	quit = 1;
      }
    }
  }
  
  // kill the window and clean up
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
