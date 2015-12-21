#include "SndEffect.h"
#include "Util.h"

#include<SDL_Mixer.h>

// Function to delete Mix_Chunks
void DeleteChunk(Mix_Chunk * chunk){
    Mix_FreeChunk(chunk);
}

// Create Mix_Chunk, turn into ChunkPtr with deleter
ChunkPtr LoadChunk(std::string fileName){
    std::string path = SOUND_DIR + fileName;
    Mix_Chunk * chnk = Mix_LoadWAV(path.c_str());
    
    if (chnk == nullptr)
        return nullptr;
    
    return ChunkPtr(chnk, DeleteChunk);
}

// Constexpr, will handle more formats hopefully
size_t DefaultSampleSize() {
    switch(MIX_DEFAULT_FORMAT){
        case AUDIO_S16SYS:
        default:
            return sizeof(short);
    }
    return 1;
}

// Get duration of sound chunk in seconds
float GetChunkDur(Mix_Chunk * chnk){
    if (chnk == nullptr)
        return -1.f;
    uint32_t nSamples = chnk->alen / DefaultSampleSize();
    return float(nSamples) / MIX_DEFAULT_FREQUENCY;
}

// Default ctor
SndEffect::SndEffect():
    m_fDuration(0)
{}

// Construct chunk from filename
// Negative duration if invalid
SndEffect::SndEffect(std::string name):
    m_Chunk(LoadChunk(name)),
    m_fDuration(GetChunkDur(m_Chunk.get()))
{}

// Getters
Mix_Chunk * SndEffect::GetChunk() const{
    return m_Chunk.get();
}

float SndEffect::GetDuration() const{
    return m_fDuration;
}
