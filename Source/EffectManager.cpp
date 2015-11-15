#include "EffectManager.h"

#include "Util.h"

#include <SDL_Mixer.h>

EffectManager::PlayingSound::PlayingSound(const SndEffect * pSnd /*= nullptr*/, int chanNum /* = -1 */):
    m_pSndEffect(pSnd),
    m_fTimePlayed(pSnd ? pSnd->GetDuration() : 0),
    m_nChanID(chanNum)
{}

float EffectManager::PlayingSound::GetPlayedRatio() const{
    if (m_pSndEffect)
        return m_fTimePlayed / m_pSndEffect->GetDuration();
    return -1.f;
}

void EffectManager::PlayingSound::Tick(float delT){
    if (m_pSndEffect){
        m_fTimePlayed -= delT;
        //std::cout << m_fTimePlayed << ", " << delT << std::endl;
        if (m_fTimePlayed < 0.f){
            m_pSndEffect = nullptr;
            m_fTimePlayed = 0.f;
        }
    }
}

bool EffectManager::PlayingSound::StillLive() const{
    return m_pSndEffect != nullptr;
}

void EffectManager::PlayingSound::Halt(){
    if (m_nChanID >= 0)
        Mix_HaltChannel(m_nChanID);
    m_fTimePlayed = 0.f;
    m_pSndEffect = nullptr;
    m_nChanID = -1;
}

EffectManager::EffectManager(int nChannels /* = 0 */):
    m_nChannels(nChannels),
    m_fLastTime(timeAsFloat(Time::now()))
{
    // Given the # of SDL Channels, split them out amongst the priorities
    m_PriorityChannels[SndEff_P::ALPHA].resize(nChannels);
    m_PriorityOffset[SndEff_P::ALPHA] = 0;
}

bool EffectManager::RegisterEffect(std::string effName){
    SndEffect sE(effName);
    if (sE.GetChunk() == nullptr)
        return false;
    m_EffectMap.emplace(effName, std::move(sE));
    return true;
}

// Bear in mind that a channel can play multiple effects at once...
bool EffectManager::PlaySound(int iPriority, std::string effName){
    // Cast to enum, find best match (lower bound?)
    SndEff_P priority = static_cast<SndEff_P>(std::max(0, iPriority));
    auto offsetIt = m_PriorityOffset.find(priority);
    if (offsetIt == m_PriorityOffset.end())
        std::advance(offsetIt, -1);
    
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
        itFree = std::min_element(sndVec.begin(), sndVec.end(), [](const PlayingSound& a, const PlayingSound& b){
            return a.GetPlayedRatio() < b.GetPlayedRatio();
        });
        itFree->Halt();
    }
    
    *itFree = PlayingSound(&itEff->second);
    
    int chanNum = m_PriorityOffset[priority] + std::distance(sndVec.begin(), itFree);
    Mix_PlayChannel(chanNum, itEff->second.GetChunk(), 0);

    //std::cout << chanNum << std::endl;
    
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
        std::vector<PlayingSound>& psVec = el.second;
        for (auto psIt = psVec.begin(); psIt != psVec.end(); ++psIt){
            if (psIt->StillLive()){
                psIt->Tick(del * 0.001f);
                if (psIt->StillLive() == false)
                    psIt->Halt();
            }
        }
    }
    
    m_fLastTime = tNow;
}