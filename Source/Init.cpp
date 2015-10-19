
#include <fstream>
#include <glm/gtx/transform.hpp>

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
	bool success = (SDL_Init(SDL_INIT_VIDEO) >= 0);
	if (success == false)
		std::cout << SDL_GetError() << std::endl;
	return success;
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
	return (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0);
}

// TODO 
// make this not look like shit
// Add global entity sequence PyLiaison module
bool InitPython() {
	// Expose camera
	Python::Register_Class<Camera>("Camera");
	std::function<void(Camera *, vec2, vec2, vec2)> cam_InitOrtho(&Camera::InitOrtho);
	Python::Register_Mem_Function<Camera, struct CamInitOrtho_t>("InitOrtho", cam_InitOrtho, "Initialize Ortho Camera with lr/tb/nf");

	//// Register Entity
	Python::Register_Class<Entity>("Entity");
	std::function<bool(Entity *, int, int)> ent_HandleMsg(&Entity::HandleMessage);
	Python::Register_Mem_Function<Entity, struct EntHandleVoidMsg_t>("PostMessage", ent_HandleMsg, "Post a message to the Entity's queue");
	
	std::function<bool(Entity *, int, int, Python::Object)> ent_HandleRequest(&Entity::HandleRequest);
	Python::Register_Mem_Function<Entity, struct EntHandleMsg_t>("PostMessage_v4", ent_HandleRequest, "Post a message to the Entity's queue");

	Python::Register_Function<struct AudPlaySnd_t>("PlaySound", Python::make_function(Audio::PlaySound), "Play a sound file");

	// Expose Drawable
	Python::Register_Class<Drawable>("Drawable");
	std::function<void(Drawable *, vec3)> dr_Translate(&Drawable::Translate);
	Python::Register_Mem_Function<Drawable, struct DrTrans_t>("Translate", dr_Translate, "Translate a drawable");
	std::function<void(Drawable *, vec4)> dr_SetColor(&Drawable::SetColor);
	Python::Register_Mem_Function<Drawable, struct DrSetColor_t>("SetColor", dr_SetColor, "Translate a drawable");


	// Register Circle class (nothing to expose, really)
	Python::Register_Class<Circle>("Circle");

    // Init python
	Python::initialize();
    
    // Expose some useful constants to the PyLiaison module
    
    // Directories
    Python::GetPyLiaisonModule().set_attr("RES_DIR", RES_DIR);
    Python::GetPyLiaisonModule().set_attr("SND_DIR", RelPathToAbs(SOUND_DIR));
    Python::GetPyLiaisonModule().set_attr("SHD_DIR", SHADER_DIR);
    Python::GetPyLiaisonModule().set_attr("MOD_DIR", MODEL_DIR);
    
    // Entity messages
    Python::GetPyLiaisonModule().set_attr("E_COL", int(Entity::CompID::COLLISION));
    Python::GetPyLiaisonModule().set_attr("E_DR", int(Entity::CompID::DRAWABLE));
    Python::GetPyLiaisonModule().set_attr("E_DR_TR", int(Entity::MsgID::DR_TRANSLATE));
    Python::GetPyLiaisonModule().set_attr("E_DR_CLR", int(Entity::MsgID::DR_COLOR));
    
    // This map isn't actually used, but as long as an empty
    // PyDict gets created I'm fine with it
    std::map<uint32_t, Entity *> dummyMap;
    Python::GetPyLiaisonModule().set_attr("g_Entities", dummyMap);
    
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