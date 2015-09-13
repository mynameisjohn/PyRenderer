#include "Init.h"
#include "Util.h"

#include "GL_Includes.h"

#include "pyliason.h"

#include <SDL.h>

#include <gtx/transform.hpp>

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

	return true;
}

bool InitScene(Scene& s) {
	// Get the python init module
	std::string pyinitScript = SCRIPT_DIR + "init.py";
	auto pyinitModule = Python::Object::from_script(pyinitScript);

	// Get all the info you need
	using float2 = std::array<float, 2>;
	using float3 = std::array<float, 3>;
	using float4 = std::array<float, 4>;
	using EntInfo = std::tuple<float3, float3, float4, float4, std::string>;
	auto check = [](bool cond, std::string msg = "") {
		if (!msg.empty())
			std::cout << msg << "----" << (cond ? " failed! " : " succeeded! ") <<  std::endl;
		assert(cond);
	};

	std::vector<EntInfo> v_EntInfo;
	auto pyl_EntInfo = pyinitModule.call_function("GetEntities");
	check(pyl_EntInfo.convert(v_EntInfo), "Getting all Entity info");


	std::map<std::string, Python::Object> map_CachedPyModules;

	for (auto& ei : v_EntInfo) {
		vec3 pos(std::get<0>(ei)[0], std::get<0>(ei)[1], std::get<0>(ei)[2]);
		vec3 scale(std::get<1>(ei)[0], std::get<1>(ei)[1], std::get<1>(ei)[2]);
		fquat rot(std::get<2>(ei)[0], std::get<2>(ei)[1], std::get<2>(ei)[2], std::get<2>(ei)[3]);
		vec4 color(std::get<3>(ei)[0], std::get<3>(ei)[1], std::get<3>(ei)[2], std::get<3>(ei)[3]);
		std::string module = std::get<4>(ei);

		// Load the python module
		map_CachedPyModules[module] = std::move(Python::Object::from_script(module));

		// Get drawable info
		mat4 MV = glm::translate(pos) * glm::mat4_cast(rot) * glm::scale(scale);
		color = glm::clamp(color, vec4(0), vec4(1));
		//std::string iqmFile;
		//check(map_CachedPyModules[module].get_attr("r_IqmFile").convert(iqmFile), "Getting IqmFile from module "+module);

		// Get collision info
		//std::array<float2, 2> a_ColInfo;
		//check(map_CachedPyModules[module].get_attr("r_Collider").convert(a_ColInfo), "Getting IqmFile from module " + module);
		//vec2 c_Pos(a_ColInfo[0][0], a_ColInfo[0][1]);
		//vec2 c_Pos(a_ColInfo[1][0], a_ColInfo[1][1]);
	}

	return true;
}