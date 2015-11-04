#include "Util.h"
#include "Init.h"
#include "Audible.h"
#include <SDL.h>

std::unique_ptr<Scene> sPtr(nullptr);
// OpenGL context and main window
SDL_GLContext g_Context;
SDL_Window * g_Window = nullptr;

int main(int argc, char ** argv) {
	// Start up OpenGL, SDL, Python, load resources
	if (!InitEverything(g_Context, g_Window, sPtr))
		return EXIT_FAILURE;
	SDL_Event e;
    
	// Play/pause
    enum PlayState{
        PLAY,
        PAUSE,
        STEP_FWD,
        STEP_REV,
        QUIT
    };

	// Current state, iterations per step
    PlayState curState = PLAY;
    const int nStep = 1;

	while (curState != QUIT) {
		// TODO Python event handler
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_KEYDOWN:
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					curState = QUIT;
                        break;
                    case SDLK_SPACE:
                        if (curState != PLAY){
                            globalTimeStep = dT;
                            curState = PLAY;
                        }
                        else
                            curState = PAUSE;
                        break;
                case SDLK_RIGHT:
                        if (curState == PAUSE){
                            globalTimeStep = dT;
                            for (int i=0; i< nStep; i++)
                                sPtr->Update();
                        }
                        break;
                case SDLK_LEFT:
                        if (curState == PAUSE){
                            globalTimeStep = -dT;
                            for (int i=0; i< nStep; i++)
                                sPtr->Update();
                        }
                        break;
				}
			}
			}
		}
        
		// Update the scene
        if (curState == PLAY)
            sPtr->Update();
		
		// Update screen, draw (I don't care if it's paused)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sPtr->Draw();
		SDL_GL_SwapWindow(g_Window);
	}

	return EXIT_SUCCESS;
}