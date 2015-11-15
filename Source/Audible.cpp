#include "Audible.h"

#include <SDL_mixer.h>


namespace Audio {
	std::map<std::string, ChunkPtr> s_ChunkMap;

    //std::map<std::string, SndEffect> s_MapSndEffects;
    
	int PlaySound(std::string soundFile) {
		if (s_ChunkMap.find(soundFile) == s_ChunkMap.end()) {
			std::cout << "Error: attempt made to play unloaded sound file " << soundFile << std::endl;
			if (!LoadSound(soundFile))
				return -1;
		}
        
		//Mix_PlayChannel(-1, s_ChunkMap[soundFile].get(), 0);
		//std::cout << "Playing sound " << soundFile << std::endl;
		//std::cout <<  << std::endl;
		return 0;
	}

	bool LoadSound(std::string fileName) {
		if (s_ChunkMap.find(fileName) == s_ChunkMap.end()) {
			std::string path = SOUND_DIR + fileName;
			//std::string key = fileName.substr(0, fileName.rfind('.'));
			Mix_Chunk * chnk = Mix_LoadWAV(path.c_str());
			if (chnk) {
				s_ChunkMap[fileName] = std::move(ChunkPtr(chnk));
				return true;
			}
			std::cout << "Error: unable to load sound file " << fileName << "\n" << Mix_GetError() << std::endl << "\n" << std::endl;;
		}
		return false;
	}
}