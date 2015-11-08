
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
#include "InputManager.h"

bool InitEverything(SDL_GLContext& g_Context, SDL_Window*& g_Window, std::unique_ptr<Scene>& pScene) {
	if (InitSDL())
		if (InitGL(g_Context, g_Window))
			if (InitSound())
				if (InitPython())
					if (InitScene(pScene))
						return true;
	return false;
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
	std::function<int(Entity *, int, int)> ent_SendMsg(&Entity::SendMessage);
	Python::Register_Mem_Function<Entity, struct EntHandleVoidMsg_t>("SendMessage", ent_SendMsg, "Post a message to the Entity's queue");
	
	std::function<int(Entity *, int, int, Python::Object)> ent_SendMsg_D(&Entity::SendMessage_D);
	Python::Register_Mem_Function<Entity, struct ent_SendMsg_D>("SendMessage_D", ent_SendMsg_D, "Post a message to the Entity's queue");

	Python::Register_Function<struct AudPlaySnd_t>("PlaySound", Python::make_function(Audio::PlaySound), "Play a sound file");

	// Expose Drawable
	Python::Register_Class<Drawable>("Drawable");
	std::function<void(Drawable *, vec3)> dr_Translate(&Drawable::Translate);
	Python::Register_Mem_Function<Drawable, struct DrTrans_t>("Translate", dr_Translate, "Translate a drawable");
	std::function<void(Drawable *, vec4)> dr_SetColor(&Drawable::SetColor);
	Python::Register_Mem_Function<Drawable, struct DrSetColor_t>("SetColor", dr_SetColor, "Translate a drawable");

	// Register Circle class (nothing to expose, really)
	Python::Register_Class<Circle>("Circle");
    
    Python::Register_Class<InputManager>("InputManager");
    std::function<bool(InputManager *, int)> inMgr_IsKeyDwn(&InputManager::IsKeyDown);
    Python::Register_Mem_Function<InputManager, struct inIsKeyDwn_t>("IsKeyDown", inMgr_IsKeyDwn, "Check if key is down");
    std::function<int(InputManager *, int)> inMgr_GetKey(&InputManager::GetKeyState);
    Python::Register_Mem_Function<InputManager, struct inIsKeyDwn_t>("GetKeyState", inMgr_GetKey, "Check if key is down");
    
    // Init python
	Python::initialize();
    
    // Expose some useful constants to the PyLiaison module
    Python::Object PYL = Python::GetPyLiaisonModule();
    
    // Key Codes, this is unforunate (put in separate module)
    PYL.set_attr<int>("K_SPACE", SDLK_SPACE);
    PYL.set_attr<int>("K_RIGHT", SDLK_RIGHT);
    PYL.set_attr<int>("K_c", SDLK_c);
    
    // Directories
    PYL.set_attr("RES_DIR", RES_DIR);
    PYL.set_attr("SND_DIR", RelPathToAbs(SOUND_DIR));
    PYL.set_attr("SHD_DIR", SHADER_DIR);
    PYL.set_attr("MOD_DIR", MODEL_DIR);
    
    // Entity messages
    PYL.set_attr("E_COL", int(Entity::CompID::COLLISION));
    PYL.set_attr("E_DR", int(Entity::CompID::DRAWABLE));
    PYL.set_attr("E_DR_TR", int(Entity::MsgID::DR_TRANSLATE));
    PYL.set_attr("E_DR_CLR", int(Entity::MsgID::DR_COLOR));
    
    // This map isn't actually used, but as long as an empty
    // PyDict gets created I'm fine with it
    std::map<uint32_t, Entity *> dummyMap;
    PYL.set_attr("g_Entities", dummyMap);
    
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
Scene::Scene(std::string& pyinitScript) :
m_ContactSolver(100)
{
    auto check = [](bool cond, std::string msg = "") {
        if (!msg.empty())
            std::cout << msg << "----" << (cond ? " succeeded! " : " failed! ") << std::endl;
        assert(cond);
    };
    
    // Convert the relative python path to the absolute, load module
    std::string initStrPath = FixBackslash(RelPathToAbs(SCRIPT_DIR ) + "/" + pyinitScript);
    Python::Object pyinitModule = Python::Object::from_script(initStrPath);
    
    // Get main module name and load it
    std::string mainModName;
    pyinitModule.call_function("GetMainModuleName").convert(mainModName);
    m_MainPyModule = Python::Object::from_script(mainModName);
    
    // Set up the shader
    std::map<std::string, std::string> shaderInfo;
    pyinitModule.call_function("GetShaderSrc").convert(shaderInfo);
    m_Shader = Shader(shaderInfo["vert"], shaderInfo["frag"], SHADER_DIR);
    
    // Get position handle
    auto sBind = m_Shader.ScopeBind();
    GLint posHandle = m_Shader[shaderInfo["Position"]];
    Drawable::SetPosHandle(posHandle);
    
    // Initialize Camera
    pyinitModule.call_function("InitCamera", &m_Camera);
    
    // Like a mini factory (no collision info for now, just circles)
    using EntInfo = std::tuple<
    vec2, // velocity
    vec2, // Position
    vec2, // Scale
    float, // Rotation about +z
    float, // mass
    float, // elasticity
    vec4, // Color
    std::string>;
    
    // Now loop through all named tuples in the script
    std::vector<EntInfo> v_EntInfo;
    pyinitModule.call_function("GetEntities").convert(v_EntInfo);
    
    // Preallocate all entities, since we'll need their pointers
    m_vEntities.resize(v_EntInfo.size());
    
    // Create individual entities and components
    for (int i = 0; i < m_vEntities.size(); i++) {
        // Get the tuple
        auto ei = v_EntInfo[i];
        
        // Unpack tuple
        vec2 vel(std::get<0>(ei));
        vec3 pos(std::get<1>(ei), 0.f);
        vec2 scale(std::get<2>(ei));
        float rot = std::get<3>(ei);
        float m = std::get<4>(ei);
        float e = clamp(std::get<5>(ei),0.f,1.f);
        vec4 color = glm::clamp(std::get<6>(ei), vec4(0), vec4(1));
        std::string pyEntModScript = std::get<7>(ei);
        
        // Load the python module
        auto pyEntModule = Python::Object::from_script(SCRIPT_DIR + pyEntModScript);
        
        // Create Entity
        Entity ent(i, this, pyEntModule);
        m_vEntities[i] = ent;
        
        // Get the entity's resources as a tuple (wow)
        std::tuple<std::string, std::string, std::list<std::string> > eRsrc;
        pyEntModule.call_function("GetResources").convert(eRsrc);
        
        // IQM File
        std::string iqmFile = std::get<0>(eRsrc);
//        check(pyEntModule.get_attr("r_IqmFile").convert(iqmFile), "Getting IqmFile from module " + pyEntModScript);
        
        
        // Collision primitives (this will get more complicated)
        std::string colPrim = std::get<1>(eRsrc);
       // check(pyEntModule.get_attr("r_ColPrim").convert(colPrim), "Getting basic collision primitive from ent module");
        
        // Make collision resource, (assume uniform scale, used for mass and r)
        // TODO add mass, elasticity to init tuple
        if (colPrim == "AABB") {  // AABBs are assumed to be "walls" of high mass for now
            AABB box(vel, vec2(pos), m, e, scale);
            box.SetEntity(&m_vEntities[i]);
            m_vAABB.push_back(box);
        }
        else if (colPrim == "OBB") {
            OBB box(vel, vec2(pos), m, e, scale, rot);
            box.SetEntity(&m_vEntities[i]);
            //box.w = (omegaDir++ % 2 ? -10.f : 0.f);
            m_vOBB.push_back(box);
        }
        else {
            Circle circ(vel, vec2(pos), m, e, maxEl(scale));
            circ.SetEntity(&m_vEntities[i]);;
            m_vCircles.push_back(circ);
        }
    
        // Sounds
        std::list<std::string> sndFiles = std::get<2>(eRsrc);
        //check(pyEntModule.get_attr("r_Sounds").convert(sndFiles), "Getting all sounds from module " + pyEntModScript);
        for (auto& file : sndFiles)
            Audio::LoadSound(file);
        
        // Make drawable
        fquat rotQ(cos(rot / 2.f), vec3(0, 0, sin(rot / 2.f)));
        Drawable dr(iqmFile, color, quatvec(pos, rotQ), scale);
        dr.SetEntity(&m_vEntities[i]);
        m_vDrawables.push_back(dr);
    }
    
    // Fix entity pointers (I hate this)
    for (auto& circle : m_vCircles)
        circle.GetEntity()->SetColCmp(&circle);
    for (auto& box : m_vAABB)
        box.GetEntity()->SetColCmp(&box);
    for (auto& box : m_vOBB)
        box.GetEntity()->SetColCmp(&box);
    for (auto& drawable : m_vDrawables)
        drawable.GetEntity()->SetDrCmp(&drawable);
    
    // Expose in python, mapping ent ID to Exposed Entity
    // TODO Entities should be globally accessible via the PyLiaison module,
    // so find a way fo adding a container to it
    // PyDict_New...
    for (auto& ent : m_vEntities)
        ent.GetPyModule().call_function("AddEntity", ent.GetID(), &ent);
}