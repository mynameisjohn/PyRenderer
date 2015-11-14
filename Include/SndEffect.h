#pragma once

#include <memory>

struct Mix_Chunk;
struct ChunkDeleter {
    void operator()(Mix_Chunk * chunk);
};
using ChunkPtr = std::unique_ptr<Mix_Chunk, ChunkDeleter>;

class SndEffect{
public:
    // Standard constructors
    SndEffect();
    SndEffect(std::string);
    
    // Copy/move constructors
    SndEffect(const SndEffect&);
    SndEffect(const SndEffect&&);
    
    // Copy/move assignments
    SndEffect& operator=(const SndEffect&);
    SndEffect& operator=(const SndEffect&&);
    
    // Default destructor
    ~SndEffect() = default;
    
    // Comparison
    //    bool operator==(const SndEffect&);
    //    bool operator<(const SndEffect& other);
    
    Mix_Chunk * GetChunk() const;
    float GetDuration() const;
private:
    ChunkPtr m_Chunk;
    float m_fDuration;
};