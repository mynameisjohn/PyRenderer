#include "Util.h"
#include "Init.h"

#include <SDL.h>

std::unique_ptr<Scene> sPtr(nullptr);

int main(int argc, char ** argv) {
	if (!InitGL())
		return EXIT_FAILURE;

	if (!InitPython())
		return EXIT_FAILURE;

	if (!InitScene(sPtr))
		return EXIT_FAILURE;

	if (sPtr == nullptr)
		return EXIT_FAILURE;

	bool quit(false);
	SDL_Event e;

	while (!quit) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_KEYDOWN:
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
				}
			}
			}
		}

		sPtr->Draw();
	}
	return EXIT_SUCCESS;
}