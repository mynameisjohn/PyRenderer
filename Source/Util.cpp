#ifdef _WIN32
// For path stuff
#include <Windows.h>

// For algorithm
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#else
// Unix path stuff
#include <stdlib.h>

#endif

#include "Util.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// time step declaration
float globalTimeStep = 0.005f;

// Definitions of glm print functions
std::ostream& operator<<(std::ostream& os, const vec2& vec) {
	os << "{" << vec.x << ", " << vec.y << "}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const vec3& vec) {
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const vec4& vec) {
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const mat4& mat) {
	mat4 transMat = glm::transpose(mat);
	os << "{\n" << transMat[0] << ",\n" << transMat[1] << ",\n" << transMat[2] << ",\n" << transMat[3] << ",\n}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const fquat& quat) {
	os << "{" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << "}";
	return os;
}

std::string RelPathToAbs(const std::string relPath) {
	char buf[2048];

#ifdef _WIN32
	::GetFullPathName(relPath.c_str(), 1000, buf, 0);
#else
    realpath(relPath.c_str(), &buf[0]);
#endif

	return std::string(buf);
}

std::string FixBackslash(const std::string& in) {
	std::string out(in);
	std::replace(out.begin(), out.end(), '\\', '/');
	return out;
}

bool feq(float a, float b, float diff){
    return fabs(a-b) < diff;
}

float maxEl(vec2 v) {
	float ret(0.f);
	for (int i = 0; i < 2; i++)
		ret = std::max(ret, v[i]);
	return ret;
}

float maxEl(vec3 v) {
	float ret(0.f);
	for (int i = 0; i < 3; i++)
		ret = std::max(ret, v[i]);
	return ret;
}

float maxEl(vec4 v) {
	float ret(0.f);
	for (int i = 0; i < 4; i++)
		ret = std::max(ret, v[i]);
	return ret;
}

float minEl(vec2 v) {
	float ret(0.f);
	for (int i = 0; i < 2; i++)
		ret = std::min(ret, v[i]);
	return ret;
}

float minEl(vec3 v) {
	float ret(0.f);
	for (int i = 0; i < 3; i++)
		ret = std::min(ret, v[i]);
	return ret;
}

float minEl(vec4 v) {
	float ret(0.f);
	for (int i = 0; i < 4; i++)
		ret = std::min(ret, v[i]);
	return ret;
}


vec2 perp(vec2 v){
    return vec2(-v.y, v.x);
}

vec2 maxComp(vec2 v){
    if (feq(v.x, v.y))
        return v;
    else if (fabs(v.x) > fabs(v.y))
        return vec2(v.x, 0.f);
    else
        return vec2(0.f, v.y);
}

//This returns a rotation quat that will line something
//up with the given vec2 in x,y (meaning rotation is about z)
fquat getQuatFromVec2(vec2 r) {
	r = glm::normalize(r);
	float s(sqrt((1 - r.x) / 2));
	float c((r.y>0) ? sqrt((1 + r.x) / 2) : -sqrt((1 + r.x) / 2));

	return fquat(c, s*vec3(0, 0, 1));
}

float timeAsFloat(Time::time_point tp){
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    return duration_cast<milliseconds>(tp.time_since_epoch()).count();
}