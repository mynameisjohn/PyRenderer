
#include <fstream>
#include <gtx/transform.hpp>

#include <pyliason.h>
#include <SDL.h>

#include "pyl_overloads.h"
#include "Init.h"
#include "Util.h"
#include "GL_Includes.h"

#include <SDL_mixer.h>
#include "Audible.h"


bool InitEverything(SDL_GLContext& g_Context, SDL_Window*& g_Window, std::unique_ptr<Scene>& pScene) {
	if (!InitSDL())
		return false;
	if (!InitGL(g_Context, g_Window))
		return false;
	if (!InitSound())
		return false;
	if (!InitPython())
		return false;
	if (!InitScene(pScene))
		return false;
	return true;
}

bool InitSDL() {
	return (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0);
}

bool InitGL(SDL_GLContext& g_Context, SDL_Window*& g_Window) {
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

bool InitSound() {
	return (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0);
}

// TODO 
// make this not look like shit
// Add global entity sequence PyLiaison module
bool InitPython() {
	// Expose camera
	Python::Register_Class<Camera>("Camera");
	std::function<void(Camera *, vec2, vec2, vec2)> cam_InitOrtho(&Camera::InitOrtho);
	Python::Register_Mem_Function<Camera, __LINE__>("InitOrtho", cam_InitOrtho, "Initialize Ortho Camera with lr/tb/nf");

	// Register Entity
	Python::Register_Class<Entity>("Entity");
	std::function<bool(Entity *, int, int)> ent_PostMessage(&Entity::PostMessage);
	Python::Register_Mem_Function<Entity, __LINE__>("PostMessage", ent_PostMessage, "Post a message to the Entity's queue");
	
	std::function<bool(Entity *, int, int, vec4)> ent_PostMessage_v4(&Entity::PostMessage<vec4>);
	//Python::Register_Mem_Function<Entity, __LINE__>("PostMessage_v4", ent_PostMessage_v4, "Post a message to the Entity's queue");

	std::function<int(std::string)> playFn(Audio::PlaySound);
	//Python::Register_Function<__LINE__>("PlaySound", playFn);
	//Py_Add_Func("PlaySound", Audio::PlaySound, "Play a sound file");

	// Expose Drawable
	Python::Register_Class<Drawable>("Drawable");
	//std::function<void(Drawable *, vec3)> dr_Translate(&Drawable::Translate);
	//Python::Register_Mem_Function<Drawable, __LINE__>("Translate", dr_Translate, "Translate a drawable");
	//std::function<void(Drawable *, vec4)> dr_SetColor(&Drawable::SetColor);
	//Python::Register_Mem_Function<Drawable, __LINE__>("Translate", dr_SetColor, "Translate a drawable");


	// Register Circle class (nothing to expose, really)
	Python::Register_Class<Circle>("Circle");

	// Abstract interface to GetEntity, if you need it
	//Python::Register_Class<PyComponent>("Component");
	//std::function<Entity *(PyComponent *)> pyc_GetEntity(&PyComponent::GetEntity);
	//Python::Register_Mem_Function<PyComponent, __LINE__>("GetEntity", pyc_GetEntity, "Get the entity pointer");

	Python::initialize();

	return true;
}

// This is really backward. The Scene constructor is long and gross.
// Python is what should be driving everything. There should be an AddEntity
// function exposed that does this stuff. I don't know... it all seems bad. 
bool InitScene(std::unique_ptr<Scene>& pScene) {
	// Get the python init module
	std::string pyinitScript = "InitScene.py";

	try { pScene = std::move(std::unique_ptr<Scene>(new Scene(pyinitScript))); }
	catch (...) { return false; }

	return true;
}

// This shouldn't go here... but it's so long