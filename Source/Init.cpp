#include "Init.h"
#include "Util.h"

#include "GL_Includes.h"

#include "pyliason.h"

#include <SDL.h>

// Screen dims
const int WIDTH = 600;
const int HEIGHT = 600;

// GL version
const int glMajor(3), glMinor(0);

// OpenGL context and main window
SDL_GLContext g_Context;
SDL_Window * g_Window = nullptr;

bool InitGL() {
	//Init SDL Video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Init SDL+OpenGL Context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Create Window
	g_Window = SDL_CreateWindow("3D Test",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (g_Window == NULL) {
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Create Context from Window
	g_Context = SDL_GL_CreateContext(g_Window);
	if (g_Context == NULL) {
		printf("OpenGL context was not created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		return false;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	//OpenGL settings
	glClearColor(0.15f, 0.15f, 0.15f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE_ARB);

	//These are legacy calls valid in OpenGL 3.0 only
#ifndef __APPLE__
	glAlphaFunc(GL_GREATER, 0.9f);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

	//For debugging
	glLineWidth(8.f);

	return true;
}


bool InitPython() {
	// Do your shit
	//Py_Add_Func("InitScene", InitScene, "Init Scene with a correctly formatted XML File");
	Python::initialize();

	//std::function<decltype(Foo::nothing)> f(&Foo::nothing);
	//Python::Register_Function<__LINE__>("Sdf", std::function<int(Foo *)>(&Foo::nothing), 0);
	//Python::Register_Class<Camera, __LINE__>("Camera");
	//Python::RegFn_RA<__LINE__>("dsf", f, METH_VARARGS);
	//std::function<void(Camera *, float, float, float, float, float, float)> cam_InitOrtho(&Camera::InitOrtho);
	//Python::RegMemFn_VA<__LINE__, Camera>("InitOrtho", cam_InitOrtho, METH_VARARGS, "Initialize Ortho Camera with lr/tb/nf");
	//Python::Register_Function<__LINE__, Camera>("InitOrtho", cam_InitOrtho, METH_VARARGS, "Initialize Ortho Camera with lr/tb/nf");

	return true;
}

bool InitScene(std::unique_ptr<Scene>& pScene) {
	// Get the python init module
	std::string pyinitScript = SCRIPT_DIR + "init.py";
	pScene = std::move(std::unique_ptr<Scene>(new Scene(pyinitScript)));
	
	return true;
}