#include <iostream>
#include <string>
#include <format>
#include <SDL.h>
#include "PlayStation3.hpp"

#ifdef _WIN32
#include <windows.h>
// Gently ask to use the discrete Nvidia/AMD GPU if possible instead of
// integrated graphics
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
#endif


static constexpr double MS_PER_FRAME = 1000.0 / 60.0;

SDL_GameController* findController() {
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            return SDL_GameControllerOpen(i);
        }
    }

    return nullptr;
}

int main(int argc, char** argv) {
    if (argc < 2)
        Helpers::panic("Usage: ChonkyStation3.exe [executable path]");

    printf("ChonkyStation3\n\n");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
        Helpers::panic("Failed to initialize SDL\n");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
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

    PlayStation3* ps3 = new PlayStation3(file);

    printf("\nEXECUTING\n");
    printf(  "---------\n\n");

    bool quit = false;
    bool fullscreen = false;
    bool vsync_enabled = false;

    int frame_count = 0;
    double last_time = SDL_GetTicks64() / 1000.0;
    double curr_time = 0;
    float ppu_usage = 0;

    SDL_GL_SwapWindow(window);

    SDL_GameController* controller = findController();

    while (!quit) {
        ps3->run();
        ps3->module_manager.cellGcmSys.flip = 0;

        frame_count++;

        const u64 curr_ticks = SDL_GetTicks64();
        curr_time = curr_ticks / 1000.0;

        if (curr_time - last_time > 1.0) {
            ppu_usage = ((CPU_FREQ - ps3->skipped_cycles) * 100.0f) / CPU_FREQ;
            title = std::format("ChonkyStation3 | {} | {} FPS | PPU: {:.2f}%", file.filename().string(), frame_count, std::ceil(ppu_usage * 100.0f) / 100.0f);
            SDL_SetWindowTitle(window, title.c_str());
            last_time = curr_time;
            frame_count = 0;
        }

        ps3->resetButtons();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    quit = true;
                    break;
                }

                case SDL_MOUSEBUTTONDOWN: {
                    if (e.button.button == SDL_BUTTON_LEFT && e.button.clicks == 2) {
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
                        SDL_ShowCursor(fullscreen ? SDL_DISABLE : SDL_ENABLE);
                    } else if (e.button.button == SDL_BUTTON_RIGHT) {
                        vsync_enabled = !vsync_enabled;
                        SDL_GL_SetSwapInterval(vsync_enabled ? 1 : 0);
                    }
                    break;
                }

                case SDL_CONTROLLERDEVICEADDED: {
                    if (!controller) {
                        controller = SDL_GameControllerOpen(e.cdevice.which);
                    }
                    break;
                }
                case SDL_CONTROLLERDEVICEREMOVED: {
                    if (controller && e.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
                        SDL_GameControllerClose(controller);
                        controller = findController();
                    }
                    break;
                }
            }
        }

        if (!controller) {
            const u8* keystate = SDL_GetKeyboardState(NULL);
            if (keystate[SDL_SCANCODE_M])      ps3->pressButton(CELL_PAD_CTRL_START);
            if (keystate[SDL_SCANCODE_K])      ps3->pressButton(CELL_PAD_CTRL_CROSS);
            if (keystate[SDL_SCANCODE_L])      ps3->pressButton(CELL_PAD_CTRL_CIRCLE);
            if (keystate[SDL_SCANCODE_J])      ps3->pressButton(CELL_PAD_CTRL_SQUARE);
            if (keystate[SDL_SCANCODE_I])      ps3->pressButton(CELL_PAD_CTRL_TRIANGLE);
            if (keystate[SDL_SCANCODE_DOWN])   ps3->pressButton(CELL_PAD_CTRL_DOWN);
            if (keystate[SDL_SCANCODE_UP])     ps3->pressButton(CELL_PAD_CTRL_UP);
            if (keystate[SDL_SCANCODE_LEFT])   ps3->pressButton(CELL_PAD_CTRL_LEFT);
            if (keystate[SDL_SCANCODE_RIGHT])  ps3->pressButton(CELL_PAD_CTRL_RIGHT);
        }
        else {
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START))             ps3->pressButton(CELL_PAD_CTRL_START);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A))                 ps3->pressButton(CELL_PAD_CTRL_CROSS);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B))                 ps3->pressButton(CELL_PAD_CTRL_CIRCLE);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X))                 ps3->pressButton(CELL_PAD_CTRL_SQUARE);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y))                 ps3->pressButton(CELL_PAD_CTRL_TRIANGLE);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN))         ps3->pressButton(CELL_PAD_CTRL_DOWN);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP))           ps3->pressButton(CELL_PAD_CTRL_UP);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT))         ps3->pressButton(CELL_PAD_CTRL_LEFT);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT))        ps3->pressButton(CELL_PAD_CTRL_RIGHT);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER))      ps3->pressButton(CELL_PAD_CTRL_L1);
            if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))     ps3->pressButton(CELL_PAD_CTRL_R1);
            if (SDL_GameControllerGetAxis(controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 20000)     ps3->pressButton(CELL_PAD_CTRL_L2);
            if (SDL_GameControllerGetAxis(controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 20000)    ps3->pressButton(CELL_PAD_CTRL_R2);
        }

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
