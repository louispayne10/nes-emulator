#include <SDL2/SDL.h>

#include "cartridge.h"
#include "cpu.h"
#include "log.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fmt::print("usage: <program_name> <path_to_rom>");
        return -1;
    }
    CPU6502 cpu;

    auto cart = Cartridge::from_file(argv[1]);
    if (!cart.has_value()) {
        info_message("failed to load cart");
    }
#if 0
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("An SDL2 window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480,
                                          SDL_WINDOW_OPENGL);
    if (!window) {
        DEBUG_LOG("could not create window: {}", SDL_GetError());
        return EXIT_FAILURE;
    }

    // The window is open: could enter program loop here (see SDL_PollEvent())

    SDL_Delay(3000); // Pause execution for 3000 milliseconds, for example

    SDL_DestroyWindow(window);
    SDL_Quit();
#endif
}
