#pragma once

#include "SndEffect.h"

#include <map>
#include <vector>
#include <string>

enum class SndEff_P : uint32_t {
    ALPHA=0
};

class EffectManager{
    class PlayingSound{
        const SndEffect * m_pSndEffect;
        float m_fTimePlayed;
    public:
        PlayingSound(const SndEffect * pSnd = nullptr);
        float GetPlayedRatio() const;
        void Tick(float delT);
        bool StillLive() const;
    };
    
    using EffectPriorityQ = std::map<SndEff_P, std::vector<PlayingSound> >;
    
    EffectPriorityQ m_PriorityChannels;
    uint32_t m_nChannels;
    float m_fLastTime;
    std::map<std::string, SndEffect> m_EffectMap;
public:
    EffectManager(int nChannels = 0);\
    void Update();
    bool RegisterEffect(std::string effName);
    bool PlaySound(SndEff_P, std::string);
    void DeadToFront();
};