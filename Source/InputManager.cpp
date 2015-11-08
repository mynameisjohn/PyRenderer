#include "InputManager.h"

#include <SDL_events.h>

int keyCode(SDL_KeyboardEvent * ke){
    return ke->keysym.sym;
}

InputManager::InputManager():
    m_ScreenDim(1),
    m_MousePos(0),
    m_DeltaMouse(0)
{}

//KeyState::KeyState(SDL_KeyboardEvent * ke):
//    repeat(ke->repeat),
//    key(keyCode(ke))
//{}
//        
//KeyState::KeyState(int k):
//key(k)
//{}

void InputManager::setKeyState(SDL_KeyboardEvent * ke){
    int k = keyCode(ke);
    auto it = m_KeyState.find(k);
    if (it == m_KeyState.end())
        m_KeyState.emplace(k, 1);
    else if (ke->repeat)
        clearKeyState(ke);
}

void InputManager::clearKeyState(SDL_KeyboardEvent *ke){
    int k = keyCode(ke);
    auto it = m_KeyState.find(k);
    if (it != m_KeyState.end())
        m_KeyState.erase(it);
}

void InputManager::setMousePos(int x, int y){
    vec2 oldPos = m_MousePos;
    m_MousePos = vec2(x, y);
    m_DeltaMouse = m_MousePos - oldPos;
}

vec2 InputManager::GetScreenDim() const{
    return m_ScreenDim;
}

vec2 InputManager::GetMousePos() const{
    return m_MousePos;
}

vec2 InputManager::GetDeltaMouse() const{
    return m_DeltaMouse;
}

bool InputManager::IsKeyDown(int k) const{
    return (m_KeyState.find(k) != m_KeyState.end());
}

int InputManager::GetKeyState(int k) const{
    auto it = m_KeyState.find(k);
    if (it != m_KeyState.end())
        return it->second;
    return 0;
}

// Don't deal with repeat for now
void InputManager::HandleEvent(SDL_Event * e){
    switch (e->type) {
        case SDL_KEYDOWN:
            setKeyState(&e->key);
            break;
        case SDL_KEYUP:
            clearKeyState(&e->key);
            break;
        case SDL_MOUSEMOTION:
            setMousePos(e->motion.x, e->motion.y);
            break;
        // Mouse Button?
    }
    
}