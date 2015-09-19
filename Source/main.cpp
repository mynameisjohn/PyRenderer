#include "Util.h"
#include "Init.h"

#include <SDL.h>

std::unique_ptr<Scene> sPtr(nullptr);
// OpenGL context and main window
SDL_GLContext g_Context;
SDL_Window * g_Window = nullptr;

int main(int argc, char ** argv) {
	if (!InitGL(g_Context, g_Window))
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sPtr->Draw();
		SDL_GL_SwapWindow(g_Window);
	}
	return EXIT_SUCCESS;
}