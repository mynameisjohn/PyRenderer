#include "Entity.h"
#include "Drawable.h"
#include "RigidBody.h"

Entity::Entity(uint32_t id, Circle * cCmp, Drawable * drCmp) :
	m_UniqueID(id),
	m_ColCmp(cCmp),
	m_DrCmp(drCmp)
{}

bool Entity::PyExpose(const std::string& name, PyObject * module) {
	Python::Expose_Object(this, name, module);
	return true;
}

bool Entity::PostMessage(int C, int M) {
	bool handled = false;

	switch (C) {
	case int(CompID::DRAWABLE): // These casts are unfortunate
		switch (M) {
		case int(MsgID::TRANSLATE) : {
			const auto drPtr = m_DrCmp;
			const vec2 lastT = m_ColCmp->lastT;
			// Get last translation, apply, reset
			m_MessageQ.emplace_back([drPtr, lastT]() {
				drPtr->Translate(vec3(lastT, 0.f));
				return true;
			});
			//	std::bind(&Drawable::Translate, m_DrCmp, m_ColCmp->lastT));
			m_ColCmp->lastT = vec2(0.f);
			handled = 0;
		}
		}
	}

	return handled;
}

// Just run all posted messages
void Entity::Update() {
	for (auto& msg : m_MessageQ)
		msg();
}

Python::Object Entity::GetPyModule()  const {
	return m_PyModule;
}

int Entity::GetID() const {
	return m_UniqueID;
}

// I'm putting this here, but I don't know why
PyComponent::PyComponent(Entity * ePtr) :
	m_pEntity(ePtr)
{}

Entity * PyComponent::GetEntity()  const {
	return m_pEntity;
}