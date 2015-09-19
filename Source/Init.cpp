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
	// Do your shit
	//Py_Add_Func("InitScene", InitScene, "Init Scene with a correctly formatted XML File");

	Python::Register_Class<Camera, __LINE__>("Camera");
	std::function<int(Camera *, vec2, vec2, vec2)> cam_InitOrtho(&Camera::InitOrtho);
	Python::Register_Mem_Function<Camera, __LINE__>("InitOrtho", cam_InitOrtho, "Initialize Ortho Camera with lr/tb/nf");

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
	std::string pyinitScript = "InitScene.py";

	try {
		pScene = std::move(std::unique_ptr<Scene>(new Scene(pyinitScript)));
	}
	catch (...) {

		char arr[] = "path";
		PyObject *path(PySys_GetObject(arr));
		std::string p;
		Python::convert(path, p);

		return false;
	}
	return true;
}

#include <fstream>
#include <gtx/transform.hpp>
#include <stdlib.h>
#include <Windows.h>

Scene::Scene(std::string& pyinitScript) {
	auto check = [](bool cond, std::string msg = "") {
		if (!msg.empty())
			std::cout << msg << "----" << (cond ? " succeeded! " : " failed! ") << std::endl;
		assert(cond);
	};

	char buf[1000];
	::GetFullPathName((SCRIPT_DIR + pyinitScript).c_str(), 1000, buf, 0);
	std::string strBuf(buf);
	std::replace(strBuf.begin(), strBuf.end(), '\\', '/');

	auto pyinitModule = Python::Object::from_script(strBuf);

	// First deal with the shader and camera

	// Exposing shader functions just seemed unnecessary
	std::map<std::string, std::string> shaderInfo;
	check(pyinitModule.get_attr("r_ShaderSrc").convert(shaderInfo), "Getting shader info from pyinit module " + pyinitScript);
	m_Shader = Shader(shaderInfo["vert"], shaderInfo["frag"], SHADER_DIR);

	auto sBind = m_Shader.ScopeBind();
	GLint posHandle = m_Shader[shaderInfo["Position"]];
	Drawable::SetPosHandle(posHandle);

	// InitOrtho / InitPersp should be exposed
	//Python::Expose_Object(&m_Camera, "c_Camera", pyinitModule.get());
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
		m_PyObjCache[pyEntModScript] = std::move(pyEntModule);

		//check(map_CachedPyModules[module].get_attr("r_IqmFile").convert(iqmFile), "Getting IqmFile from module "+module);

		// Get collision info
		//std::array<float2, 2> a_ColInfo;
		//check(map_CachedPyModules[module].get_attr("r_Collider").convert(a_ColInfo), "Getting IqmFile from module " + module);
		//vec2 c_Pos(a_ColInfo[0][0], a_ColInfo[0][1]);
		//vec2 c_Pos(a_ColInfo[1][0], a_ColInfo[1][1]);

		int entId(m_vEntities.size());
		int entDrOfs(m_vDrawables.size());
		m_vEntities.emplace_back(entId, entDrOfs, -1);
		m_vDrawables.emplace_back(iqmFile, color, MV);
	}
}
