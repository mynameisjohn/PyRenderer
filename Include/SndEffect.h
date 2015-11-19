#pragma once

#include <memory>

// This used to be a unique_ptr
// I opted out because I don't trust or
// really want my move syntax right now,
// but I could easily cache these off and
// instance them.
struct Mix_Chunk;
using ChunkPtr = std::shared_ptr<Mix_Chunk>;
ChunkPtr MakeChunkPtr(Mix_Chunk *);

// Basically a wrapper class for Mix_Chunk
// Right now it just caches off duration as a
// float (in seconds, I believe). It would be
// cool to store format as well to do some
// real DSP, but I really don't need it right now
class SndEffect{
public:
    // Standard constructors
    SndEffect();
    SndEffect(std::string);
    
    // Default destructor
    ~SndEffect() = default;
    
    Mix_Chunk * GetChunk() const;
    float GetDuration() const;
private:
    ChunkPtr m_Chunk;
    float m_fDuration;
};