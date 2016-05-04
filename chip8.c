#include <SDL2/SDL.h>
#include <inttypes.h>
#include <stdio.h>
#include "nfd.h"
#include "common.h"
#include "nfd_common.h"
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "chip8.h"
#include "cpu.h"

// Function prototypes:
int start_video(SDL_Window**, SDL_Surface**);
int load_font(struct emu_vars*);
int open_program(struct emu_vars*, SDL_Window*);
//int cpu_cycle()

int main(int argc, char* argv[]) {
    
    // SDL and control flow Variables:
    SDL_Window* window;  // Window pointer
    SDL_Surface* screen; // Screen pointer
    SDL_Event e;         // Event handler
    int quit = 0;        // Quit flag
    int run_emu = 0;     // Flag tells whether or not CPU should run code
    const uint8_t *state = SDL_GetKeyboardState(NULL); // Keyboard state array

    // Emulation Variables:
    struct emu_vars hw_state;
    hw_state.opcode = 0; // Zero the current opcode
    hw_state.I = 0;      // Zero Index Register
    hw_state.pc = 0x200; // Start at 0x200 (512)
    hw_state.sp = 0;     // Zero stack pointer
    
    // Call start_video with pointers to the screen and window references:
    if( start_video(&window, &screen) != 0 ) {
	return 1;
    }
    if( load_font(&hw_state) != 0 ) {
	return 1;
    }

    // Clear display, stack, and V registers
    CLS(&hw_state); // Display
    for(int i = 0; i < 16; i++) {
      hw_state.stack[i] = 0; // Stack
      hw_state.V[i]=0; // V registers
    }
    
    
    // Main loop:
    while( !quit ) {

	// Check for open key:
	if (state[SDL_SCANCODE_O]) {
	  printf("O is pressed\n");
	  while(state[SDL_SCANCODE_O]) {
	    SDL_PumpEvents();
	  }
	  if(open_program(&hw_state, window) != 0) {
	    return 1;
	  }
	  run_emu = 0;
	}
	// Check for pause key:
	if (state[SDL_SCANCODE_P]) {
	  printf("P is pressed\n");
	  while(state[SDL_SCANCODE_P]) {
	    SDL_PumpEvents();
	  }
	  if(run_emu) {
	    run_emu = 0;
	    printf("Paused\n");
	  } else {
	    run_emu = 1;
	    printf("Unpaused\n");
	  }
	}

	if(run_emu){
	  //cpu_runcycle(&hw_state);
	  hw_state.pc++;
	}
	
     	// Update the screen:
	SDL_UpdateWindowSurface( window );
	
	// Check for events:
	while( SDL_PollEvent( &e ) != 0 ) {
	    // User requests quit:
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

/* 
   This function accepts a window and a screen by reference.  It then
   initializes them and makes the screen black.  It is called only at the start
   of the program.
*/
int start_video(SDL_Window** window, SDL_Surface** screen) {
	
    SDL_Init(SDL_INIT_VIDEO);
    
    *window = SDL_CreateWindow(
	"CHIP 8 Emulator",                 // window title
	SDL_WINDOWPOS_UNDEFINED,           // initial x position
	SDL_WINDOWPOS_UNDEFINED,           // initial y position
	640,                               // width, in pixels
	320,                               // height, in pixels
	SDL_WINDOW_OPENGL                  // flags
	);
    
    if (*window == NULL) {
	// In the case that the window could not be made...
	printf("Could not create window: %s\n", SDL_GetError());
	return 1;
    }
    
    // Set the surface on which we will render the screen
    *screen = SDL_GetWindowSurface( *window );
    
    // Fill it with black
    SDL_FillRect(
	*screen,
	NULL,
	SDL_MapRGB( (*screen)->format, 0, 0, 0 )
	);

    return 0;
}

int load_font(struct emu_vars* hw_state) {
    // CHIP8 fontset:
    uint8_t chip8_fontset[80] =
	{ 
	    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	    0x20, 0x60, 0x20, 0x20, 0x70, // 1
	    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

    // load it into the CHIP8 memory
    for(int i = 0; i < 80; ++i) {
	hw_state->memory[i] = chip8_fontset[i];
    }
    
    return 0;
}

int open_program(struct emu_vars* hw_state, SDL_Window* window) {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );
    if ( result == NFD_OKAY ) {
	struct stat st;
	stat(outPath, &st);
	printf("%lu\n", st.st_size);
	if(st.st_size > 3000) {
	   return 1;
	}
	// load into memory
	FILE *fp;
	unsigned char buffer[3000];
	for(int i=0; i<3000; i++) {
	    buffer[i]=0;
	}
	fp=fopen(outPath, "rb");
	if (fp) {
	    fread(buffer, 3000, 1, fp);
	    fclose(fp);
	    for(int i = 0; i < 3000; ++i) {
	      hw_state->memory[i + 512] = buffer[i];
	    }
	}
	else {
	    return 1;
	}
	return 0;
    }
    else if ( result == NFD_CANCEL ) {
	printf("cancel\n");
	return 0;
    }
    else {
        printf("Error: %s\n", NFD_GetError() );
	return 1;
    }
}


