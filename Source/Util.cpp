#include "Util.h"

#include <glm.hpp>
#include <gtc/quaternion.hpp>

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