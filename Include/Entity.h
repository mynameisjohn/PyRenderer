#pragma once

#include "Drawable.h"

#include "Util.h"

class Entity {
	int m_UniqueId;
	int m_ofsDrawable;
	int m_ofsCollider;
public:
	Entity(int id, int drId, int cId) :
		m_UniqueId(id),
		m_ofsDrawable(drId),
		m_ofsCollider(cId)
	{}
};