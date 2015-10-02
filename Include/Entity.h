#pragma once

#include <pyliason.h>
#include "Util.h"
#include <vec4.hpp>

class Entity;

// I'm conflicted by this
// On one hand it's cumbersome to add inheritance
// to things and feels like a house of cards
// on the other hand using a class like this means I only have to
// write the code once, and I don't need any overrides in other files
class OwnedByEnt {
protected:
	Entity * m_pEntity;
public:
	OwnedByEnt();
	OwnedByEnt(Entity * pEnt);
	Entity * GetEntity() const;
	uint32_t GetEntID() const;
	void SetEntity(Entity * const pEnt);
};

class Drawable;
class RigidBody_2D;
class Scene;

class Entity {
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

private:
	int m_UniqueID;
	RigidBody_2D * m_ColCmp;
	Drawable * m_DrCmp;
	Scene * m_Scene;
	MessageQueue m_MessageQ;
	Python::Object m_PyModule;


protected:
	friend class Scene;
	void SetColCmp(RigidBody_2D * colPtr);
	void SetDrCmp(Drawable * drPtr);

public:
	Entity();
	Entity(int id, Scene * scnPtr, Python::Object module);
	
	Python::Object GetPyModule()  const;
	int GetID() const;
	
	// Message handling stuff
	template <typename D>
	bool PostMessage(int C, int M, D data);

	// Void (no data) case
	bool PostMessage(int C, int M);

	// Who knows what it'll be?
	//bool PostMessage(int C, int M, Python::Object data);

	void Update();
};