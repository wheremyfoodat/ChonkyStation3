﻿#include <iostream>
#include <string>
#include <format>
#include <SDL.h>
#include "PlayStation3.hpp"


static constexpr double MS_PER_FRAME = 1000.0 / 60.0;

int main(int argc, char** argv) {
    if (argc < 2)
        Helpers::panic("Usage: ChonkyStation3.exe [executable path]");

    printf("ChonkyStation3\n\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        Helpers::panic("Failed to initialize SDL\n");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    
    fs::path file = argv[1];
    std::string title = std::format("ChonkyStation3 | {}", file.filename().string());

    SDL_Window* window = SDL_CreateWindow(title.c_str(), 100, 100, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(0);

    if (context == nullptr)
        Helpers::panic("Failed to create OpenGL context\n");

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        Helpers::panic("OpenGL init failed");
    }

    PlayStation3 ps3 = PlayStation3(file);

    printf("\nEXECUTING\n");
    printf(  "---------\n\n");

    bool quit = false;
    int frame_count = 0;
    double last_time = SDL_GetTicks64() / 1000.0;
    double curr_time = 0;

    while (!quit) {
        ps3.run();
        ps3.module_manager.cellGcmSys.flip = 0;

        frame_count++;

        const u64 curr_ticks = SDL_GetTicks64();
        curr_time = curr_ticks / 1000.0;

        if (curr_time - last_time > 1.0) {
            title = std::format("ChonkyStation3 | {} | {} FPS", file.filename().string(), frame_count);
            SDL_SetWindowTitle(window, title.c_str());
            last_time = curr_time;
            frame_count = 0;
        }

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
