#ifndef UTIL_H
#define UTIL_H

// Util.h
// Useful utility functions and includes that get knocked around often


// They'll get included somewhere anyway
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <memory>
#include <stdint.h>

// MSVC is for chumps
#define _USE_MATH_DEFINES
#include <math.h>

// Stringify macros
#define xstr(s) str(s)
#define str(s) #s

// I'm moving the time step here
// and making it non-const, so that
// I can go back and forth in time
const static float dT = 0.005f;
extern float globalTimeStep;
// Smol
const float kEPS = 0.001f;

// glm is important to me
#include <glm/fwd.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::fquat;
using glm::mat4;
using glm::mat3;

// As are these printing functions
#include <iostream>
std::ostream& operator<<(std::ostream& os, const vec2& vec);
std::ostream& operator<<(std::ostream& os, const vec3& vec);
std::ostream& operator<<(std::ostream& os, const vec4& vec);
std::ostream& operator<<(std::ostream& os, const mat4& mat);
std::ostream& operator<<(std::ostream& os, const fquat& quat);

// Useful dirs
#if defined(__APPLE__)
const std::string RES_DIR = "../../Resources/";
#else
const std::string RES_DIR = "../Resources/";
#endif

const std::string SHADER_DIR = RES_DIR + "Shaders/";
const std::string SCRIPT_DIR = RES_DIR + "Scripts/";
const std::string MODEL_DIR = RES_DIR + "Models/";
const std::string SOUND_DIR = RES_DIR + "Sounds/";

// RelPathToAbs returns the absolute path of a file, given its relative path
std::string RelPathToAbs(const std::string relPath);

// Replace windows slashes with unix
std::string FixBackslash(const std::string& in);

// General purpose functions
//template <typename T>
//inline bool strAppnd(T& v, std::string s) {
//	return (bool)(std::stringstream(s) >> v);
//}

bool feq(float a, float b, float diff = kEPS);

template <typename T>
inline T remap(T v, T m1, T m2, T m3, T m4) {
	return (((v - m1) / (m2 - m1))*(m4 - m3) + m3);
}

template <typename T>
inline T clamp(T v, T m, T M) {
	return std::min<T>(std::max<T>(v, m), M);
}

inline float degToRad(float d)  {
	return float(d * M_PI / 180.f);
}

inline float radToDeg(float r)  {
	return float(r * 180.f / M_PI);
}

vec2 perp(vec2 v);
vec2 maxComp(vec2 v);

float maxEl(vec2 v);
float maxEl(vec3 v);
float maxEl(vec4 v);

float minEl(vec2 v);
float minEl(vec3 v);
float minEl(vec4 v);

inline uint32_t wrapIdx(int dim, int idx)  {
	return (dim + idx) % dim;
}

using Time = std::chrono::high_resolution_clock;

float timeAsFloat(Time::time_point tp);

//This returns a rotation quat that will line something
//up with the given vec2 in x,y (meaning rotation is about z)
fquat getQuatFromVec2(vec2 r);

// Always useful
class StopWatch {
    decltype(Time::now()) m_Begin, m_End;
	std::string m_Name;
public:
	StopWatch(std::string s) :
        m_Begin(Time::now()),
		m_Name(s) {
	}
	~StopWatch() {
		using std::chrono::duration_cast;
		using std::chrono::milliseconds;
        m_End = Time::now();
		std::cout << m_Name << " took " << duration_cast<milliseconds>(m_End - m_Begin).count() << " mS";
	}
};

#endif //UTIL_H