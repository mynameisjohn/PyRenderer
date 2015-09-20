#pragma once

#include <pyliason.h>

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
		TRANSLATE
	};

	// Maybe not the best idea
	friend class Scene;
protected:
	uint32_t m_UniqueID;
	Circle * m_ColCmp;
	Drawable * m_DrCmp;
	MessageQueue m_MessageQ;
	Python::Object m_PyModule;
public:
	Entity(uint32_t id = -1, Circle * cCmp = nullptr, Drawable * drCmp = nullptr);
	
	Python::Object GetPyModule()  const;
	
	// Message handling stuff
	void PostMessage(CompID C, MsgID M);
	void Update();

	// Python expose override
	bool PyExpose(const std::string& name, PyObject * module) override;
};

// Python exposed component (Why is this here?)
class PyComponent : public PyExposed {
protected:
	Entity * m_pEntity;
public:
	PyComponent(Entity * ePtr);
	Entity * GetEntity() const;
	virtual bool PyUpdate() = 0;
};