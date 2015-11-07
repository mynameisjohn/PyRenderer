#pragma once

#include "Util.h"

#include <set>
#include <glm/vec2.hpp>

union SDL_Event;
struct SDL_KeyboardEvent;

struct KeyState{
    KeyState(SDL_KeyboardEvent * ke);
    KeyState(int k);
    bool repeat;
    int key;
    struct comp{
        bool operator()(const KeyState& a, const KeyState& b){
            return a.key < b.key;
        }
    };
    
    bool operator<(const KeyState& other){
        return key < other.key;
    }
    bool operator==(const KeyState& other){
        return key == other.key;
    }
};

class InputManager{
    vec2 m_ScreenDim;
    vec2 m_MousePos;
    vec2 m_DeltaMouse;
    std::set<KeyState, KeyState::comp> m_KeyState;
    
    void setKeyState(SDL_KeyboardEvent * ke);
    void clearKeyState(SDL_KeyboardEvent * ke);
    void setMousePos(int x, int y);
public:
    InputManager();
    void HandleEvent(SDL_Event * e);
    bool IsKeyDown(int k) const;
    vec2 GetScreenDim() const;
    vec2 GetMousePos() const;
    vec2 GetDeltaMouse() const;
};