
#include <fstream>
#include <gtx/transform.hpp>
#include <stdlib.h>
#include <Windows.h>


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

bool InitGL(SDL_GLContext& g_Context, SDL_Window*& g_Window) {
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

namespace Python
{
	bool convert(PyObject * o, vec2& v) {
		return convert_buf(o, &v[0], 2);
	}
	bool convert(PyObject * o, vec3& v) {
		return convert_buf(o, &v[0], 3);
	}
	bool convert(PyObject * o, vec4& v) {
		return convert_buf(o, &v[0], 4);
	}
	bool convert(PyObject * o, fquat& v) {
		return convert_buf(o, &v[0], 4);
	}
}

bool InitPython() {
	Python::Register_Class<Camera, __LINE__>("Camera");
	std::function<int(Camera *, vec2, vec2, vec2)> cam_InitOrtho(&Camera::InitOrtho);
	Python::Register_Mem_Function<Camera, __LINE__>("InitOrtho", cam_InitOrtho, "Initialize Ortho Camera with lr/tb/nf");

	Python::Register_Class<Drawable, __LINE__>("Drawable");
	std::function<void(Drawable *, vec3)> dr_Translate(&Drawable::Translate);
	Python::Register_Mem_Function<Drawable, __LINE__>("Translate", dr_Translate, "Translate a drawable");

	Python::Register_Class<Circle, __LINE__>("Circle");

	Python::initialize();

	return true;
}

bool InitScene(std::unique_ptr<Scene>& pScene) {
	// Get the python init module
	std::string pyinitScript = "InitScene.py";

	try { pScene = std::move(std::unique_ptr<Scene>(new Scene(pyinitScript))); }
	catch (...) { return false; }

	return true;
}

Scene::Scene(std::string& pyinitScript) {
	auto check = [](bool cond, std::string msg = "") {
		if (!msg.empty())
			std::cout << msg << "----" << (cond ? " succeeded! " : " failed! ") << std::endl;
		assert(cond);
	};

	std::string initStrPath = RelPathToAbs(SCRIPT_DIR + pyinitScript);
	auto pyinitModule = Python::Object::from_script(initStrPath);

	// First deal with the shader and camera

	// Exposing shader functions just seemed unnecessary
	std::map<std::string, std::string> shaderInfo;
	check(pyinitModule.get_attr("r_ShaderSrc").convert(shaderInfo), "Getting shader info from pyinit module " + pyinitScript);
	m_Shader = Shader(shaderInfo["vert"], shaderInfo["frag"], SHADER_DIR);

	// Get position handle
	auto sBind = m_Shader.ScopeBind();
	GLint posHandle = m_Shader[shaderInfo["Position"]];
	Drawable::SetPosHandle(posHandle);

	// InitOrtho / InitPersp should be exposed
	pyinitModule.call_function("InitCamera", &m_Camera);

	// Now loop through all named tuples in the script
	// script must be correct...

	// Get a vector of these
	using EntInfo = std::tuple<vec3, vec3, fquat, vec4, std::string>;
	std::vector<EntInfo> v_EntInfo;
	check(pyinitModule.get_attr("r_Entities").convert(v_EntInfo), "Getting all Entity Info");
	//check(pyl_EntInfo.convert(v_EntInfo), "Getting all Entity info");

	// Once you have it, create individual entities and components

	for (auto& ei : v_EntInfo) {

		// Get python module
		std::string pyEntModScript = std::get<4>(ei);
		auto pyEntModule = Python::Object::from_script(SCRIPT_DIR + pyEntModScript);

		// Get drawable info
		mat4 MV = glm::translate(std::get<0>(ei)) * glm::mat4_cast(std::get<2>(ei)) * glm::scale(std::get<1>(ei));
		vec4 color = glm::clamp(std::get<3>(ei), vec4(0), vec4(1));
		std::string iqmFile;
		check(pyEntModule.get_attr("r_IqmFile").convert(iqmFile), "Getting IqmFile from module " + pyEntModScript);
		m_PyObjCache[pyEntModScript] = pyEntModule;

		//check(map_CachedPyModules[module].get_attr("r_IqmFile").convert(iqmFile), "Getting IqmFile from module "+module);

		// Drawable inifo
		Drawable dr(iqmFile, color, MV);

		// Get collision info
		float radius = std::get<1>(ei)[0]; // Assume uniform scale for now...
		vec2 c(std::get<0>(ei));
		Circle circ;
		circ.C = c;
		circ.r = radius;
		circ.V = -0.5f*c;
		circ.m = 1.f;
		circ.e = 1.f;

		int uID(m_vEntities.size());
		int drID(m_vDrawables.size());
		int cID(m_vCircles.size());

		m_vDrawables.push_back(dr);
		m_vCircles.push_back(circ);
		m_vEntities.emplace_back(uID, drID, cID, pyEntModule);
	}

	for (auto& ent : m_vEntities) {
		auto drPtr = &m_vDrawables[ent.m_ofsDrawable];
		auto cPtr = &m_vCircles[ent.m_ofsCollider];
		ent.m_PyModule.call_function("AddEntity", drPtr, cPtr);
		drPtr->PyInit(ent.m_UniqueId, ent.m_PyModule);
		cPtr->PyInit(ent.m_UniqueId, ent.m_PyModule);
	}
}
