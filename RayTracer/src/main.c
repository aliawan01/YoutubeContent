#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef double f64;
typedef float f32;

typedef u32 b32;

#define true 1
#define false 0

#define ArrayCount(x) (sizeof(x)/sizeof((x)[0]))
#define EachIndex(i, max) (u32 i = 0; i < max; i++)

typedef struct {
  u8* pixels;
  u32 width;
  u32 height;
  u32 bytes_per_pixel;
} ScreenInfo;

int main(int argc, char** args) {
	if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_EVENTS) < 0) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
		return 1;
	} 

	SDL_Window* window = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);

	if (!window) {
		printf("Error creating window: %s\n", SDL_GetError());
		return 1;
	}

  SDL_Event event;
  b32 running = true;
  while (running) {
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT: {
          running = false;
        } break;
        case SDL_KEYDOWN: { 
          printf("Pressed down key\n");
        } break;
        case SDL_KEYUP: {
          printf("pressed up key\n");
        } break;
      }
    }


    SDL_Surface* winSurface = SDL_GetWindowSurface(window);

    if (!winSurface) {
      printf("Error getting surface: %s\n", SDL_GetError());
      return 1;
    }

    ScreenInfo info = {
      .pixels = (u8*)winSurface->pixels,
      .width = winSurface->w,
      .height = winSurface->h,
      .bytes_per_pixel = winSurface->format->BytesPerPixel
    };

    SDL_LockSurface(winSurface);

    for EachIndex(y, info.height) {
      for EachIndex(x, info.width) {
        u32 index = y*info.width*info.bytes_per_pixel + x*info.bytes_per_pixel;

        u32 colour;
        if ((x/30 + y/30) % 2 == 0)
          colour = SDL_MapRGB(winSurface->format, 255, 0, 0);
        else
          colour = SDL_MapRGB(winSurface->format, 255, 255, 255);

        *(u32*)(info.pixels+index) = colour;
      }
    }


    SDL_UnlockSurface(winSurface);


    SDL_UpdateWindowSurface(window);
  }

	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
