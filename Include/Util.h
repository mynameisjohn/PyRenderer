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
const std::string SHADER_DIR = "../Resources/Shaders/";
const std::string SCRIPT_DIR = "../Resources/Scripts/";
const std::string MODEL_DIR = "../Resources/Models/";
const std::string SOUND_DIR = "../Resources/Sounds/";

// RelPathToAbs returns the absolute path of a file, given its relative path
std::string RelPathToAbs(const std::string relPath);

// Replace windows slashes with unix
std::string FixBackslash(const std::string& in);

// General purpose functions
template <typename T>
inline bool strAppnd(T& v, std::string s) {
	return (bool)(std::stringstream(s) >> v);
}

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

float maxEl(vec2 v);
float maxEl(vec3 v);
float maxEl(vec4 v);

inline uint32_t wrapIdx(int dim, int idx)  {
	return (dim + idx) % dim;
}

inline auto timeNow() -> decltype(std::chrono::high_resolution_clock::now()) {
	return std::chrono::high_resolution_clock::now();
}


//This returns a rotation quat that will line something
//up with the given vec2 in x,y (meaning rotation is about z)
fquat getQuatFromVec2(vec2 r);

// Always useful
class StopWatch {
	decltype(timeNow()) m_Begin, m_End;
	std::string m_Name;
public:
	StopWatch(std::string s) :
		m_Begin(timeNow()),
		m_Name(s) {
	}
	~StopWatch() {
		using std::chrono::duration_cast;
		using std::chrono::milliseconds;
		m_End = timeNow();
		std::cout << m_Name << " took " << duration_cast<milliseconds>(m_End - m_Begin).count() << " mS";
	}
};

#endif //UTIL_H