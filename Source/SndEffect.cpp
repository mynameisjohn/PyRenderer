#include "SndEffect.h"
#include "Util.h"

#include<SDL_Mixer.h>

void ChunkDeleter::operator()(Mix_Chunk * chunk) {
    Mix_FreeChunk(chunk);
    chunk = nullptr;
}

ChunkPtr LoadChunk(std::string fileName){
    std::string path = SOUND_DIR + fileName;
    Mix_Chunk * chnk = Mix_LoadWAV(path.c_str());
    
    if (chnk == nullptr)
        return nullptr;
    
    return std::move(ChunkPtr(chnk));
}

size_t constexpr DefaultSampleSize() {
    switch(MIX_DEFAULT_FORMAT){
        case AUDIO_S16SYS:
        default:
            return sizeof(short);
    }
    return 1;
}

float GetChunkDur(Mix_Chunk * chnk){
    if (chnk == nullptr)
        return -1.f;
    uint32_t nSamples = chnk->alen / DefaultSampleSize();
    return float(nSamples) / MIX_DEFAULT_FREQUENCY;
    
}

// I really don't know if this is safe
ChunkPtr CopyChunk(const Mix_Chunk * copyFrom){
    if (copyFrom == nullptr)
        return nullptr;
    
    ChunkPtr ret(new Mix_Chunk);
    
    ret->alen = copyFrom->alen;
    ret->allocated = copyFrom->allocated;
    ret->volume = copyFrom->volume;
    if (copyFrom->allocated){
        ret->abuf = (Uint8 *)malloc(copyFrom->alen);
        memcpy(ret->abuf, copyFrom->abuf, ret->alen);
    }
    else
        ret->abuf = nullptr;
    
    return std::move(ret);
}

SndEffect::SndEffect():
    m_fDuration(0)
{}

SndEffect::SndEffect(std::string name):
    m_Chunk(LoadChunk(name)),
    m_fDuration(GetChunkDur(m_Chunk.get()))
{}

SndEffect::SndEffect(const SndEffect& other):
    m_Chunk(CopyChunk(other.GetChunk())),
    m_fDuration(other.GetDuration())
{}

SndEffect::SndEffect(const SndEffect&& other):
    m_Chunk((ChunkPtr&&)std::move(other.m_Chunk)),
    m_fDuration(other.GetDuration())
{}

SndEffect& SndEffect::operator=(const SndEffect & other){
    if (this!= &other){
        m_Chunk = CopyChunk(other.GetChunk());
        m_fDuration = other.GetDuration();
    }
    
    return *this;
}

SndEffect& SndEffect::operator=(const SndEffect && other){
    if (this!= &other){
        m_Chunk = (ChunkPtr&&)std::move(other.m_Chunk);
        m_fDuration = other.GetDuration();
    }
    
    return *this;
}

Mix_Chunk * SndEffect::GetChunk() const{
    return m_Chunk.get();
}

float SndEffect::GetDuration() const{
    return m_fDuration;
}
