#include "EffectManager.h"

#include "Util.h"


EffectManager::PlayingSound::PlayingSound(const SndEffect * pSnd /*= nullptr*/):
    m_pSndEffect(pSnd),
    m_fTimePlayed(0)
{}

float EffectManager::PlayingSound::GetPlayedRatio() const{
    if (m_pSndEffect)
        return m_fTimePlayed / m_pSndEffect->GetDuration();
    return -1.f;
}

void EffectManager::PlayingSound::Tick(float delT){
    m_fTimePlayed -= delT;
    if (m_fTimePlayed < 0)
        m_pSndEffect = nullptr;
}

bool EffectManager::PlayingSound::StillLive() const{
    return m_pSndEffect != nullptr;
}

EffectManager::EffectManager(int nChannels /* = 0 */):
    m_nChannels(nChannels),
    m_fLastTime(timeAsFloat(Time::now()))
{
    // Given the # of SDL Channels, split them out amongst the priorities
    m_PriorityChannels[SndEff_P::ALPHA].resize(nChannels);
}

bool EffectManager::RegisterEffect(std::string effName){
    SndEffect sE(effName);
    if (sE.GetChunk() == nullptr)
        return false;
    m_EffectMap.emplace(effName, std::move(sE));
    return true;
}

// Bear in mind that a channel can play multiple effects at once...
bool EffectManager::PlaySound(SndEff_P priority, std::string effName){
    auto itEff = m_EffectMap.find(effName);
    if (itEff == m_EffectMap.end()){
        if (!RegisterEffect(effName))
            return false;
    }
    
    std::vector<PlayingSound>& sndVec = m_PriorityChannels[priority];
    if (sndVec.empty()){
        return false;
    }
    
    auto itFree = std::find_if_not(sndVec.begin(),sndVec.end(), [](const PlayingSound& pS){
        return pS.StillLive();
    });
    if (itFree == sndVec.end()){
        auto itMin = std::min_element(sndVec.begin(), sndVec.end(), [](const PlayingSound& a, const PlayingSound& b){
            return a.GetPlayedRatio() < b.GetPlayedRatio();
        });
        *itMin = PlayingSound(&itEff->second);
    }
    
    return true;
}

void EffectManager::DeadToFront(){
    for (auto& el : m_PriorityChannels){
        std::vector<PlayingSound>& sndVec = el.second;
        std::sort(sndVec.begin(), sndVec.end(), [](const PlayingSound& a, const PlayingSound& b){
            return b.StillLive();
        });
    }
}

void EffectManager::Update(){
    float tNow(timeAsFloat(Time::now()));
    float del = tNow - m_fLastTime;
    
    for (auto& el : m_PriorityChannels){
        std::vector<PlayingSound>& sndVec = el.second;
        for (auto& pS : sndVec){
            pS.Tick(del);
        }
    }
    
    m_fLastTime = tNow;
}