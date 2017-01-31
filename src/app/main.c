
#include <stdbool.h>
#include <stdio.h>

#include "SDL2/SDL.h"

#include "vm/slip_vm.h"
#include "util/slip_io.h"


#define SLIP_APP_WINDOW_WIDTH 640
#define SLIP_APP_WINDOW_HEIGHT 480


// TODO: Clean up main function
// TODO: Move interpreter loop into main function


void
writeFn(const char* message) {
    printf(message);
}


void
assert(bool condition, const char* message) {
    if (!condition) {
        if (message == NULL) {
            printf("Assertion Failed\n");
        } else {
            printf("%s\n", message);
        }
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}


int
main(int argc, char* argv[]) {

	for (int c = 0; c < argc; c++) {
		printf("%s\n", argv[c]);
	}

	SDL_Window *win = SDL_CreateWindow("Chip-8", 100, 100,
		SLIP_APP_WINDOW_WIDTH, SLIP_APP_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	assert(win != NULL, SDL_GetError());

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert(ren != NULL, SDL_GetError());

	SDL_RendererInfo renInfo;
    assert(SDL_GetRendererInfo(ren, &renInfo) == 0, SDL_GetError());
    printf("Renderer: %s\n", renInfo.name);

    SlipConfig config;
    slipInitConfig(&config);
    config.writeFn = writeFn;

    SlipBytecode bytecode;
    slipLoadBytecode(&bytecode, "sample/hello_world.ch8");

    SlipVM* vm = slipNewVM(&config);
	slipReset(vm);
    // slipInterpretBytecode(vm, &bytecode);

	SDL_Event event;
    bool running = true;

	SDL_Rect testRect = {100, 100, 100, 100};

	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: running = false;
			}
		}

		slipStep(vm);

		SDL_SetRenderDrawColor(ren, 100, 200, 255, 255);
		SDL_RenderClear(ren);

		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderFillRect(ren, &testRect);

		SDL_RenderPresent(ren);
	}

    slipFreeVM(vm);
    slipFreeBytecode(&bytecode);

	SDL_DestroyWindow(win);
	SDL_Quit();
    return 0;
}
