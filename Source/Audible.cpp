#include "Audible.h"

namespace Audio {
	std::map<std::string, ChunkPtr> s_ChunkMap;

	void PlaySound(std::string soundFile) {
		if (s_ChunkMap.find(soundFile) == s_ChunkMap.end()) {
			std::cout << "Error: attempt made to play unloaded sound file " << soundFile << std::endl;
			if (!LoadSound(soundFile))
				return;
		}
		Mix_PlayChannel(-1, s_ChunkMap[soundFile].get(), 0);
	}

	bool LoadSound(std::string fileName) {
		if (s_ChunkMap.find(fileName) == s_ChunkMap.end()) {
			Mix_Chunk * chnk = Mix_LoadWAV(fileName.c_str());
			if (chnk) {
				s_ChunkMap[fileName] = std::move(ChunkPtr(chnk)));
				return true;
			}
			std::cout << "Error: unable to load sound file " << fileName << std::endl;
		}
		return false;
	}
}