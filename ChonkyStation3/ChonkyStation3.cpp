#include <iostream>

#include <SDL.h>

#include "PlayStation3.hpp"


int main(int argc, char** argv) {
    if (argc < 2)
        Helpers::panic("Usage: ChonkyStation3.exe [executable path]");

    printf("ChonkyStation3\n\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        Helpers::panic("Failed to initialize SDL\n");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    
    SDL_Window* window = SDL_CreateWindow("ChonkyStation3", 100, 100, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(0);

    if (context == nullptr)
        Helpers::panic("Failed to create OpenGL context\n");

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        Helpers::panic("OpenGL init failed");
    }

    PlayStation3 ps3 = PlayStation3(argv[1]);

    printf("\nEXECUTING\n");
    printf(  "---------\n\n");

    bool quit = false;
    int cycle_count = 0;

    while (!quit) {
        while (cycle_count++ < 500000) {
            ps3.step();
        }

        cycle_count = 0;

        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {

            case SDL_QUIT:
                quit = true;
                break;
            
            }
        }

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
