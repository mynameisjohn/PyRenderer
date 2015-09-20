#pragma once

#include <pyliason.h>
#include "Util.h"
#include <vec4.hpp>

// Forwards
class Circle;
class Drawable;

// Generic pythone exposed object (should this be in PyLiaison?)
class PyExposed {
public:
	virtual bool PyExpose(const std::string& name, PyObject * module) = 0;
};

class Entity : public PyExposed {
public:
	// Probably end up making a class out of this
	using MessageQueue = std::list<std::function<bool(void)>>;
	enum class CompID : int {
		DRAWABLE,
		COLLISION
	};
	enum class MsgID : int {
		DR_TRANSLATE,
		DR_COLOR
	};

	// Maybe not the best idea
	friend class Scene;
protected:
	int m_UniqueID;
	Circle * m_ColCmp;
	Drawable * m_DrCmp;
	MessageQueue m_MessageQ;
	Python::Object m_PyModule;
public:
	Entity(int id = -1, Circle * cCmp = nullptr, Drawable * drCmp = nullptr);
	
	Python::Object GetPyModule()  const;
	int GetID() const;
	
	// Message handling stuff
	template <typename D>
	bool PostMessage(int C, int M, D data);

	// Void (no data) case
	bool PostMessage(int C, int M);

	void Update();

	// Python expose override
	bool PyExpose(const std::string& name, PyObject * module) override;
};

// Python exposed component (Why is this here?)
class PyComponent : public PyExposed {
	friend class Scene;
protected:
	Entity * m_pEntity;
public:
	PyComponent(Entity * ePtr = nullptr);
	Entity * GetEntity() const;
	virtual bool PyUpdate() = 0;
};