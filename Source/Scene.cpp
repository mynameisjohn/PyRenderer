#include "Scene.h"

#include <gtx/transform.hpp>

#include <pyliason.h>

Scene::Scene(std::string& pyinitScript) {	
	auto check = [](bool cond, std::string msg = "") {
		if (!msg.empty())
			std::cout << msg << "----" << (cond ? " succeeded! " : " failed! ") << std::endl;
		assert(cond);
	};

	// Useful nicknames
	using float2 = std::array<float, 2>;
	using float3 = std::array<float, 3>;
	using float4 = std::array<float, 4>;
	using EntInfo = std::tuple<float3, float3, float4, float4, std::string>;

	auto pyinitModule = Python::Object::from_script(pyinitScript);

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
	std::vector<EntInfo> v_EntInfo;
	auto pyl_EntInfo = pyinitModule.call_function("GetEntities");
	//check(pyl_EntInfo.convert(v_EntInfo), "Getting all Entity info");

	// Once you have it, create individual entities and components

	for (auto& ei : v_EntInfo) {
		vec3 * pos = (vec3 *)(std::get<0>(ei).data());
		vec3 * scale = (vec3 *)(std::get<1>(ei).data());
		fquat * rot = (fquat *)(std::get<2>(ei).data());
		vec4 * color = (vec4 *)(std::get<3>(ei).data());

		// Get python module
		std::string pyEntModScript = std::get<4>(ei);
		auto pyEntModule = Python::Object::from_script(pyEntModScript);

		// Get drawable info
		mat4 MV = glm::translate(*pos) * glm::mat4_cast(*rot) * glm::scale(*scale);
		*color = glm::clamp(*color, vec4(0), vec4(1));
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
		m_vDrawables.emplace_back(MODEL_DIR + iqmFile, *color, MV);
	}
}

int Scene::Draw() {
	for (auto& dr : m_vDrawables)
		dr.Draw();
	return int(m_vDrawables.size());
}

//#include "Camera.h"
//#include "Shader.h"
//#include "IqmFile.h"
//#include "Textures.h"
//
//#include <string>
//#include <iostream>
//using namespace std;
//
//#include <tinyxml2.h>
//using namespace tinyxml2;
//
//#include <gtc/type_ptr.hpp>
//#include <gtx/transform.hpp>
//#include <gtc/random.hpp>
//
//#include <algorithm>
//
//// TODO : Reserve IqmFile::IQM_T::CUSTOM for extras, make a multimap
//using IqmTypeMap = map < IqmFile::IQM_T, GLint > ;
//
//GLint Scene::s_EnvMapHandle(-1);
//
//// Implementations below (you should really just make constructors outta these)
//static Geometry getGeom(XMLElement * elGeom);
//static IqmTypeMap getShader(XMLElement*elShade, ShaderPtr& sPtr, uint32_t nMat, uint32_t nLights);
//static Camera::Type getCamera(XMLElement& elCam, Camera& cam);
//static Light getLight(XMLElement * elLight);
//static void createGPUAssets(const IqmTypeMap& iqmTypes, Geometry& geom);
//static void createGPUAssets(Material& m);
//static void createGPUAssets(const Light& l);
//static void setupMiscUniforms(const ShaderPtr& sPtr);
//
//// tinyxml returns null if not found; my attempt at handling it here
//static inline float safeAtoF(XMLElement& el, string query){
//	const char * buf = el.Attribute(query.c_str());
//	if (buf)
//		return atof(buf);
//	else{
//		cout << "Error: null char ptr passed to atof" << endl;
//		// Should I really exit here?
//		exit(9);
//	}
//}
//
//// Get a child XML element, if it exists, otherwise die
//static inline XMLElement * check(string name, XMLElement * parent){
//	XMLElement * ret = parent->FirstChildElement(name.c_str());
//	if (!ret){
//		cout << "Could not find XML element " << name << endl;
//		exit(12);
//	}
//	return ret;
//}
//
//// TODO: Move constructor...
//Scene::Scene(){}
//
//Scene::Scene(string XmlSrc, ShaderPtr& sPtr, Camera& cam){
//	XMLDocument doc;
//	doc.LoadFile(XmlSrc.c_str());
//
//	// Get the root (Scene) element
//	XMLElement * elScene = doc.FirstChildElement("Scene");
//	if (!elScene){
//		cout << "XML Root not found. " << endl;
//		exit(5);
//	}
//	// Get and verify that important things are there
//	XMLElement * elCam = check("Camera", elScene);
//	XMLElement * elShade = check("Shader", elScene);
//	XMLElement * elGeom = check("Geom", elScene);
//	XMLElement * elLight = check("Light", elScene);
//
//	// First create host assets
//	// Init Camera
//	Camera::Type camType = getCamera(*elCam, cam);
//	if (camType == Camera::Type::NIL){
//		cout << "Error creating Camera" << endl;
//		exit(7);
//	}
//
//	// Init Geometry
//	for (XMLElement * el = elGeom->FirstChildElement(); el; el = el->NextSiblingElement())
//		m_vGeometry.push_back(getGeom(el));
//
//	// Set up the lights, arbitrarily assigning pre-existing geometry as its representation
//	for (auto el = elLight->FirstChildElement(); el; el = el->NextSiblingElement())
//		m_vLights.push_back(getLight(el));
//	
//	// Init shader
//	// The material count == #geom + # of point lights
//	int nMaterials = m_vGeometry.size() + std::count_if(
//		m_vLights.begin(), m_vLights.end(), [](const Light& l){
//		return l.getType() == Light::Type::POINT;
//	});
//	IqmTypeMap iqmTypes = getShader(elShade, sPtr, nMaterials, m_vLights.size());
//
//	// Sort so overlapping geometry is adjacent
//	std::sort(m_vGeometry.begin(), m_vGeometry.end());
//
//	// Create all GPU assets, starting by binding the shader
//	auto sBind = sPtr->ScopeBind();
//
//	// Create Geometry
//	for (auto it = m_vGeometry.begin(); it != m_vGeometry.end();){
//		// For the first instance, create the assets
//		createGPUAssets(iqmTypes, *it);
//		// For all remaining instances, just copy VAO and nIdx
//		int count = std::count(m_vGeometry.begin(), m_vGeometry.end(), *it);
//		auto cloneIt = it;
//		for (int i = 1; i < count; i++){
//			(it + i)->setNumIndices(it->getNumIdx());
//			(it + i)->setVAO(it->getVAO());
//		}
//		it += count;
//	}
//
//	// Set up material shader access
//	Shader * s = sPtr.get();
//	auto fixMaterial = [s](Geometry& G, int i){
//		string m = "MatArr[i].";
//		m[m.length() - 3] = '0' + i;
//
//		Material M = G.getMaterial();
//
//		// Material handles aren't static
//		M.SetReflectHandle(s->getHandle(m + "Reflectivity"));
//		M.setShinyHandle(s->getHandle(m + "Shininess"));
//		M.setDiffHandle(s->getHandle(m + "Diffuse"));
//		M.setSpecHandle(s->getHandle(m + "Specular"));
//		createGPUAssets(M);
//
//		G.setMaterial(M);
//	};
//
//	// Upload all geometry materials
//	for (int i = 0; i < m_vGeometry.size(); i++)
//		fixMaterial(m_vGeometry[i], i);
//
//	// Create lights
//	for (int i = 0; i < m_vLights.size(); i++){
//		// Upload to shader (Must be accessed like "TheLights[i].Type, GL3)
//		string s = "LightArr[i].";
//		s[s.length() - 3] = '0' + i;
//
//		// Store handles per light, since they could move
//		m_vLights[i].SetTypeHandle(sPtr->getHandle(s + "Type"));
//		m_vLights[i].SetPosOrHalfHandle(sPtr->getHandle(s + "PosOrHalf"));
//		m_vLights[i].SetDirOrAttenHandle(sPtr->getHandle(s + "DirOrAtten"));
//		m_vLights[i].SetIntensityHandle(sPtr->getHandle(s + "Intensity"));
//
//		// Put light data on GPU
//		createGPUAssets(m_vLights[i]);
//
//		// Give point lights some geometry
//		if (m_vLights[i].getType() == Light::Type::POINT){
//			Geometry lightGeom = m_vGeometry[0];
//			lightGeom.identity(); // Maybe scale it somehow?
//			mat4 light_T = glm::translate(m_vLights[i].getPos());
//			lightGeom.leftMultM(light_T);
//
//			Material lightMat(10.f, 0.f, vec4(glm::linearRand(vec3(0), vec3(1)), 0.5f), vec4(1));
//
//			lightGeom.setMaterial(lightMat);
//			fixMaterial(lightGeom, i + m_vGeometry.size() );
//			m_vLights[i].SetGeometry(lightGeom);
//		}
//
//		//// Set up point light materials
//		//if (m_vLights[i].getType() == Light::Type::POINT){
//		//	Geometry g = m_vLights[i].GetGeometry(); // I wish we didn't have to go back and forth like this
//		//	fixMaterial(g, i + m_vGeometry.size() - 1);  // but returning a reference seems like overkill
//		//	m_vLights[i].SetGeometry(g);
//		//}
//	}
//
//	setupMiscUniforms(sPtr);
//
//	Scene::s_EnvMapHandle = sPtr->getHandle("u_EnvMap");
//
//	//// Find a better place for this, do XML
//	std::string cubeFaces[6] = { "posX.png", "negX.png", "posY.png", "negY.png", "posZ.png", "negZ.png" };
//	m_EnvMap = Textures::CubeMap(cubeFaces);
//}
//#include <fstream>
//IqmTypeMap getShader(XMLElement*elShade, ShaderPtr& sPtr, uint32_t nMat, uint32_t nLights){
//	// this will store all vertex attributes
//	IqmTypeMap ret;
//
//	// Check to see if all variables described in XML are present
//	string v(check("Vertex", elShade)->Attribute("src"));
//	string f(check("Fragment", elShade)->Attribute("src"));
//
//	ifstream vIn("../Resources/Shaders/" + v), fIn("../Resources/Shaders/" + f);
//	string vSrc((istreambuf_iterator<char>(vIn)), istreambuf_iterator<char>());
//	string fSrc((istreambuf_iterator<char>(fIn)), istreambuf_iterator<char>());
//
//	// Set the light and geom (material) count in the shaders
//	auto setNum = [nMat, nLights](string& shdrSrc){
//		auto pos = shdrSrc.find("\n");
//		shdrSrc.insert(pos + 1, "#define NUM_MATS " + std::to_string(nMat) + "\n");
//		shdrSrc.insert(pos + 1, "#define NUM_LIGHTS " + std::to_string(nLights) + "\n");
//	};
//	setNum(vSrc);
//	setNum(fSrc);
//
//	sPtr = Shader::FromSource(vSrc, fSrc);
//
//	auto sBind = sPtr->Bind();
//
//	// Declared vertex attributes
//	XMLElement * attrs = check("Attributes", elShade);
//
//	// Make sure those variables exist in the shader
//	for (auto el = attrs->FirstChildElement(); el; el = el->NextSiblingElement()){
//		string type(el->Value());
//		string var(el->GetText());
//		GLint handle = sPtr->getHandle(var); // Should I have the shader ensure it's an attribute?
//		// A negative handle means the query was unsuccessful
//		if (handle < 0){
//			cout << "Invalid variable queried in shader " << var << endl;
//			continue; //exit(6);
//		}
//		// Populate returned map
//		if (type.compare("Position") == 0)
//			ret[IqmFile::IQM_T::POSITION] = handle;
//		else if (type.compare("TexCoord") == 0)
//			ret[IqmFile::IQM_T::TEXCOORD] = handle;
//		else if (type.compare("Normal") == 0)
//			ret[IqmFile::IQM_T::NORMAL] = handle;
//		else if (type.compare("Tangent") == 0)
//			ret[IqmFile::IQM_T::TANGENT] = handle;
//	}
//
//	return ret;
//}
//
//// Client must bind shader
//int Scene::Draw(){
//
//	if (m_EnvMap > 0){
//		glActiveTexture(GL_TEXTURE0 + CUBE_TEX_UNIT);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvMap);
//	}
//
//	// Draw each geom struct
//	for (int i = 0; i < m_vGeometry.size(); i++){
//		glUniform1i(Geometry::getMatIdxHandle(), i);
//		m_vGeometry[i].Draw();
//	}
//
//	// Also update and draw the lights
//	for (int i = 0; i < m_vLights.size(); i++){
//
//		// Draw point lights
//		if (m_vLights[i].getType() == Light::Type::POINT)
//			m_vLights[i].GetGeometry().Draw();
//	}
//
//	glBindVertexArray(0);
//
//	// No need for this...
//	return m_vGeometry.size();
//}
//
//// Initialize a Geometry class given an XML element, return IQM file name
//static Geometry getGeom(XMLElement *elGeom){
//	Geometry G;
//
//	XMLElement * trEl = check("Transform", elGeom);
//	vec3 T(safeAtoF(*trEl, "Tx"), safeAtoF(*trEl, "Ty"), safeAtoF(*trEl, "Tz"));
//	vec3 S(safeAtoF(*trEl, "Sx"), safeAtoF(*trEl, "Sy"), safeAtoF(*trEl, "Sz"));
//	vec3 R(safeAtoF(*trEl, "Rx"), safeAtoF(*trEl, "Ry"), safeAtoF(*trEl, "Rz"));
//	float rot = safeAtoF(*trEl, ("R"));
//	mat4 M = glm::translate(T) * glm::rotate(rot, R) * glm::scale(S);
//
//	// Create a Material
//	XMLElement * matEl = check("Material", elGeom);
//	float shininess(safeAtoF(*matEl, "shininess"));
//	float reflectivity(safeAtoF(*matEl, "reflectivity"));
//	vec4 diff(safeAtoF(*matEl, "Dr"), safeAtoF(*matEl, "Dg"), safeAtoF(*matEl, "Db"), safeAtoF(*matEl, "Da"));
//	vec4 spec(safeAtoF(*matEl, "Sr"), safeAtoF(*matEl, "Sg"), safeAtoF(*matEl, "Sb"), safeAtoF(*matEl, "Sa"));
//	Material mat(shininess, reflectivity, diff, spec);
//	if (matEl->Attribute("Texture"))
//		mat.SetTexMapSrc(matEl->Attribute("Texture"));
//	if (matEl->Attribute("Normal"))
//		mat.SetNrmMapSrc(matEl->Attribute("Normal"));
//
//	// Set values
//	G = Geometry("../Resources/IQM/" + string(elGeom->Attribute("fileName")));
//	G.leftMultM(M);
//	G.setMaterial(mat);
//
//	return G;
//}
//
//// Generate camera from XML element
//static Camera::Type getCamera(XMLElement& elCam, Camera& cam){
//	// See if ortho
//	auto test = elCam.Attribute("left");
//	if (!test){
//		test = elCam.Attribute("fovy");
//		if (!test)
//			return Camera::Type::NIL;
//
//		// Handle persp case
//		float fovy = safeAtoF(elCam, "fovy");
//		float aspect = safeAtoF(elCam, "aspect");
//		vec2 NF(safeAtoF(elCam, "near"), safeAtoF(elCam, "far"));
//		cam = Camera(fovy, aspect, NF);
//		return Camera::Type::PERSP;
//	}
//
//	// Get scene dims, set up camera
//	vec2 LR(safeAtoF(elCam, "left"), safeAtoF(elCam, "right"));
//	vec2 BT(safeAtoF(elCam, "bottom"), safeAtoF(elCam, "top"));
//	vec2 NF(safeAtoF(elCam, "near"), safeAtoF(elCam, "far"));
//	cam = Camera(LR, BT, NF);
//
//	return Camera::Type::ORTHO;
//}
//
//// Generate light from XML
//static Light getLight(XMLElement * elLight){
//	if (!elLight)
//		return Light(); // Will be Light::Type::NIL
//
//	// Position, direction (or attenuation coefs), intensity (color)
//	vec3 pos(safeAtoF(*elLight, "pX"), safeAtoF(*elLight, "pY"), safeAtoF(*elLight, "pZ"));
//	vec3 dir(safeAtoF(*elLight, "dX"), safeAtoF(*elLight, "dY"), safeAtoF(*elLight, "dZ"));
//	vec3 intensity(safeAtoF(*elLight, "iR"), safeAtoF(*elLight, "iG"), safeAtoF(*elLight, "iB"));
//
//	// If this stays nil the shader won't use it
//	string lTypeStr(elLight->Value());
//	Light::Type lType = Light::Type::NIL;
//
//	if (lTypeStr.compare("Directional") == 0)
//		lType = Light::Type::DIRECTIONAL;
//
//	if (lTypeStr.compare("Point") == 0)
//		lType = Light::Type::POINT;
//
//	if (lTypeStr.compare("Ambient") == 0)
//		lType = Light::Type::AMBIENT;
//
//	return Light(lType, pos, dir, intensity);
//}
//
//// Caller must bind shader before this can work
//static void createGPUAssets(const IqmTypeMap& iqmTypes, Geometry& geom){
//	IqmFile iqmFile(geom.GetSrcFile());
//
//	// Lambda to generate a VBO
//	auto makeVBO = []
//		(GLuint buf, GLint handle, void * ptr, GLsizeiptr numBytes, GLuint dim, GLuint type){
//		glBindBuffer(GL_ARRAY_BUFFER, buf);
//		glBufferData(GL_ARRAY_BUFFER, numBytes, ptr, GL_STATIC_DRAW);
//		glEnableVertexAttribArray(handle);
//		glVertexAttribPointer(handle, dim, type, 0, 0, 0);
//		//Disable?
//	};
//
//	// Create VAO, then create all VBOs
//	GLuint VAO(0), bIdx(0);
//	vector<GLuint> bufVBO(iqmTypes.size() + 1);
//
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glGenBuffers(bufVBO.size(), bufVBO.data());
//
//	// Get all VBO data
//	for (auto it = iqmTypes.cbegin(); it != iqmTypes.cend(); ++it){
//		switch (it->first){
//		case IqmFile::IQM_T::POSITION:
//		{
//			auto pos = iqmFile.Positions();
//			makeVBO(bufVBO[bIdx++], it->second, pos.ptr(), pos.numBytes(), pos.nativeSize() / sizeof(float), GL_FLOAT);
//		}
//		break;
//		case IqmFile::IQM_T::TEXCOORD:
//		{
//			auto tex = iqmFile.TexCoords();
//			makeVBO(bufVBO[bIdx++], it->second, tex.ptr(), tex.numBytes(), tex.nativeSize() / sizeof(float), GL_FLOAT);
//		}
//		break;
//		case IqmFile::IQM_T::NORMAL:
//		{
//			auto nrm = iqmFile.Normals();
//			makeVBO(bufVBO[bIdx++], it->second, nrm.ptr(), nrm.numBytes(), nrm.nativeSize() / sizeof(float), GL_FLOAT);
//		}
//		break;
//		case IqmFile::IQM_T::TANGENT:
//		{
//			auto tng = iqmFile.Tangents();
//			makeVBO(bufVBO[bIdx++], it->second, tng.ptr(), tng.numBytes(), tng.nativeSize() / sizeof(float), GL_FLOAT);
//		}
//		break;
//		}
//	}
//
//	// Indices
//	auto idx = iqmFile.Indices();
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufVBO[bIdx]);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.numBytes(), idx.ptr(), GL_STATIC_DRAW);
//	geom.setNumIndices(idx.count());
//
//	geom.setVAO(VAO);
//	glBindVertexArray(0);
//}
//
//static void createGPUAssets(const Light& l){
//	vec3 pos(l.getPos()), dir(l.getDir()), I(l.getIntensity());
//	glUniform1i(l.GetTypeHandle(), (int)l.getType());
//	glUniform3f(l.GetPosOrHalfHandle(), pos[0], pos[1], pos[2]);
//	glUniform3f(l.GetDirOrAttenHandle(), dir[0], dir[1], dir[2]);
//	glUniform3f(l.GetIntensityHandle(), I[0], I[1], I[2]);
//}
//
//static void createGPUAssets(Material& M){
//	// Create textures
//	std::string texMapFile = M.GetTexMapFile();
//	if (!texMapFile.empty())
//		M.SetTexMap(Textures::ColorTexture("../Resources/Textures/" + M.GetTexMapFile()));
//	else
//		M.SetTexMap(Textures::FromSolidColor(vec4(1)));
//
//	std::string nrmMapFile = M.GetNrmMapFile();
//	if (!nrmMapFile.empty())
//		M.SetNrmMap(Textures::HeightTexture("heightMap.png"));
//		//NormalTexture("../Resources/Normals/" + M.GetNrmMapFile()));
//
//	vec4 diff(M.getDiff()), spec(M.getSpec());
//	glUniform1f(M.getShinyHandle(), M.getShininess());
//	glUniform1f(M.GetReflectHandle(), M.GetReflectivity());
//	glUniform4f(M.getDiffHandle(), diff[0], diff[1], diff[2], diff[3]);
//	glUniform4f(M.getSpecHandle(), spec[0], spec[1], spec[2], spec[3]);
//}
//
//void setupMiscUniforms(const ShaderPtr& sPtr){
//	// Uniform Handles (really shouldn't be hard coded like this)
//	Camera::SetProjHandle(sPtr->getHandle("PV")); // Projection Matrix
//	Camera::SetPosHandle(sPtr->getHandle("u_wCameraPos")); // World Space cam pos
//	//Camera::SetCHandle(shader["C"]); // Camera Matrix
//
//	Geometry::setMHandle(sPtr->getHandle("M")); // World transform Matrix
//	Geometry::setNHandle(sPtr->getHandle("N")); // Normal Matrix
//	Geometry::setMatIdxHandle(sPtr->getHandle("mIdx"));
//
//	// If these end up negative, so be it
//	Material::SetTexMapHandle(sPtr->getHandle("u_TextureMap")); // Texture Map Sampler
//	Material::SetNrmMapHandle(sPtr->getHandle("u_NormalMap")); // Normal Map Sample;
//
//	// This causes GL_TEXTUREi to be associated with an int
//	// Make a manager for this inside Textures namespace
//	glUniform1i(sPtr->getHandle("u_TextureMap"), COLOR_TEX_UNIT);
//	glUniform1i(sPtr->getHandle("u_NormalMap"), NORMAL_TEX_UNIT);
//	glUniform1i(sPtr->getHandle("u_EnvMap"), CUBE_TEX_UNIT);
//}