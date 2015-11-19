#include "EffectManager.h"

#include "Util.h"

#include <SDL_Mixer.h>

// PlayingSound constructor
EffectManager::PlayingSound::PlayingSound(const SndEffect * pSnd /*= nullptr*/, int chanNum /* = -1 */):
    m_pSndEffect(pSnd),
    m_fTimePlayed(pSnd ? pSnd->GetDuration() : 0),
    m_nChanID(chanNum)
{}

// Get the percentage played (don't really need percentage...)
float EffectManager::PlayingSound::GetPlayedRatio() const{
    if (m_pSndEffect)
        return m_fTimePlayed / m_pSndEffect->GetDuration();
    return -1.f;
}

// Advance the clock
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

// Does this have anything left to give?
// pointer gets set to null when Tick sees it's over
// I feel like that's a lot of moving parts...
bool EffectManager::PlayingSound::StillLive() const{
    return m_pSndEffect != nullptr;
}

// Halt a sound effect (permanently....)
// Since ChunkPtr is now a shared pointer,
// I might let PlayingSounds own it. That
// will screw me down the road, however....
void EffectManager::PlayingSound::Halt(){
    if (m_nChanID >= 0)
        Mix_HaltChannel(m_nChanID);
    m_fTimePlayed = 0.f;
    m_pSndEffect = nullptr;
    m_nChanID = -1;
}

// Default EffectManager constructor
EffectManager::EffectManager(int nChannels /* = 0 */):
    m_nChannels(nChannels),
    m_fLastTime(timeAsFloat(Time::now()))
{
    // Given the # of SDL Channels, split them out amongst the priorities
    m_PriorityChannels[SndEff_P::ALPHA].resize(nChannels);
    m_PriorityOffset[SndEff_P::ALPHA] = 0;
}

// EffectManager caches off sound effects in a map
// Returns false if the sound effect fails to load
bool EffectManager::RegisterEffect(std::string effName){
    SndEffect sE(effName);
    if (sE.GetChunk() == nullptr)
        return false;
    m_EffectMap.emplace(effName, std::move(sE));
    return true;
}

// Bear in mind that a channel can play multiple effects at once...
bool EffectManager::PlaySound(int iPriority, std::string effName){
    // Cast to enum, find best matched priority queue (lower bound?)
    SndEff_P priority = static_cast<SndEff_P>(std::max(0, iPriority));
    auto offsetIt = m_PriorityOffset.find(priority);
    if (offsetIt == m_PriorityOffset.end())
        std::advance(offsetIt, -1);
    
    // Find the sound effect in our map
    auto itEff = m_EffectMap.find(effName);
    if (itEff == m_EffectMap.end()){
        // Sound effect may fail to load
        if (!RegisterEffect(effName))
            return false;
    }
    
    // Get the vector (representing p Q)
    std::vector<PlayingSound>& sndVec = m_PriorityChannels[priority];
    if (sndVec.empty()){
        // Channels better exist
        return false;
    }
    
    // Find the first free channel
    auto itFree = std::find_if_not(sndVec.begin(),sndVec.end(), [](const PlayingSound& pS){
        return pS.StillLive();
    });
    
    // If none were free,
    if (itFree == sndVec.end()){
        // Find the one with the highest played ratio (comparison returns true if a < b, always)
        itFree = std::max_element(sndVec.begin(), sndVec.end(), [](const PlayingSound& a, const PlayingSound& b){
            return a.GetPlayedRatio() < b.GetPlayedRatio();
        });
        itFree->Halt();
    }
    
    // Find the channel number
    int chanNum = m_PriorityOffset[priority] + std::distance(sndVec.begin(), itFree);
    
    // Replace the PlayingSound with a new one
    *itFree = PlayingSound(&itEff->second, chanNum);
    
    // Start playing the sound
    Mix_Chunk * C = itEff->second.GetChunk();
    if (C){
        Mix_PlayChannel(chanNum, C, 0);
        return true;
    }
    
    // I'm sure the check above is superfluous, but I don't crash
    return false;
}

// Sort the vectors such that any channels that have stopped playing
// are brought to the front (where std::find can get to them quicker)
void EffectManager::DeadToFront(){
    for (auto& el : m_PriorityChannels){
        std::vector<PlayingSound>& sndVec = el.second;
        std::sort(sndVec.begin(), sndVec.end(), [](const PlayingSound& a, const PlayingSound& b){
            // If a was dead and b wasn't, this returns true; if they're both the same, who cares?
            return b.StillLive();
        });
    }
}

// Update all playing sounds
void EffectManager::Update(){
    // Find how much time has passed
    float tNow(timeAsFloat(Time::now()));
    float delT = tNow - m_fLastTime;
    delT *= 0.001f; // del is in mS
    
    // For every channel queue,
    for (auto& el : m_PriorityChannels){
        std::vector<PlayingSound>& psVec = el.second;
        for (auto psIt = psVec.begin(); psIt != psVec.end(); ++psIt){
            // We definitely want to tick each sound, but...
            // If they end naturally, do we need to halt them?
            if (psIt->StillLive()){
                psIt->Tick(delT);
                if (psIt->StillLive() == false)
                    psIt->Halt();
            }
        }
    }
    
    // Update time now
    m_fLastTime = tNow;
}