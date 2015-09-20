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

void Entity::PostMessage(CompID C, MsgID M) {
	switch (C) {
	case CompID::DRAWABLE:
		switch (M) {
		case MsgID::TRANSLATE:
			// Get last translation, apply, reset
			m_MessageQ.emplace_back(
				std::bind(&Drawable::Translate, m_DrCmp, m_ColCmp->lastT));
			m_ColCmp->lastT = vec2(0.f);
		}
	}
}

// Just run all posted messages
void Entity::Update() {
	for (auto& msg : m_MessageQ)
		msg();
}

Python::Object Entity::GetPyModule()  const {
	return m_PyModule;
}

// I'm putting this here, but I don't know why
PyComponent::PyComponent(Entity * ePtr) :
	m_pEntity(ePtr)
{}

Entity * PyComponent::GetEntity()  const {
	return m_pEntity;
}