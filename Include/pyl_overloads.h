#pragma once

#include <pyliason.h>

#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <gtc/quaternion.hpp>

#include "Util.h"

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