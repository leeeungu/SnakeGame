#pragma once

#include <string>
#include "SDL_mixer.h"

namespace Sound
{
	namespace Music
	{
		enum E_DataID
		{
			E_None,
			E_Background,
			E_Title,
			E_Win,
			E_Lose,
			E_EnumMax
		};
	}

	namespace Chunk
	{
		namespace Apple
		{
			enum E_DataID
			{
				E_None,
				E_RedApple,
				E_GoldApple,
				E_BlueApple,
				E_EnumMax
			};
		}
	}
}

class SoundManager
{
private:
	SoundManager() = default;
	static SoundManager* m_pInstance;

public:

	// Singleton
	static void CreateInstance();
	static void DestroyInstance();

private:
	static std::string GetSoundPath();
	static std::string GetChunkName(Sound::Chunk::Apple::E_DataID eChunkID);
	static std::string GetMusicName(Sound::Music::E_DataID eMusicID);

public:
	// Load Sound
	static bool LoadMusic(Sound::Music::E_DataID eMusicID);
	static bool LoadChunk(Sound::Chunk::Apple::E_DataID eChunkID);

	// main music
	static void FadeInMusic(Sound::Music::E_DataID eMusicID, int loops = -1, int ms = 1000);
	static void FadeOutMusic(int ms);

	// channel chunk
	static void PlayChannel(Sound::Chunk::Apple::E_DataID eChunkID, int loops = 0, int channel = -1);

	// Setting
	static void SetVolumeMusic(int volume) { m_pInstance->m_nMusicVolume = volume; }
	static void SetVolumeChunk(int volume) { m_pInstance->m_nChunkVolume = volume; }

	// Mix_Music* GetMisic(std::string texture_id) { return music_map_[texture_id]; }
private:
	Sound::Music::E_DataID m_eMainMusic;
	Mix_Music* m_arMusic[Sound::Music::E_DataID::E_EnumMax];
	Mix_Chunk* m_archunk[Sound::Chunk::Apple::E_DataID::E_EnumMax];

	int m_nChunkVolume;
	int m_nMusicVolume;
};

