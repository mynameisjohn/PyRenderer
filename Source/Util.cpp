#include "Util.h"

#include <glm.hpp>
#include <gtc/quaternion.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

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
#endif

	return std::string(buf);
}

std::string FixBackslash(const std::string& in) {
	std::string out(in);
	std::replace(out.begin(), out.end(), '\\', '/');
	return out;
}


//This returns a rotation quat that will line something
//up with the given vec2 in x,y (meaning rotation is about z)
fquat getQuatFromVec2(vec2 r) {
	r = glm::normalize(r);
	float s(sqrt((1 - r.x) / 2));
	float c((r.y>0) ? sqrt((1 + r.x) / 2) : -sqrt((1 + r.x) / 2));

	return fquat(c, s*vec3(0, 0, 1));
}