#pragma once

#include <SDL_mixer.h>

#include "Util.h"

struct ChunkDeleter {
	void operator()(Mix_Chunk * chunk) {
		if (chunk)
			Mix_FreeChunk(chunk);
	}
};
using ChunkPtr = std::unique_ptr<Mix_Chunk, ChunkDeleter>;

// Don't feel like dealing with components at the moment
namespace Audio {
	extern std::map<std::string, ChunkPtr> s_ChunkMap;
	int PlaySound(std::string soundName); // int ID?
	bool LoadSound(std::string fileName);
}

//class Audible {
//	static std::map<std::string, ChunkPtr> s_ChunkMap;
//public:
//	Audible();
//};