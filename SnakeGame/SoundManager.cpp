#include <iostream>
#include "SoundManager.h"

SoundManager* SoundManager::m_pInstance = 0;

void SoundManager::CreateInstance() 
{
	if (m_pInstance)
		return;

	m_pInstance = new SoundManager();
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		std::cout << "Mix_OpenAudio " << Mix_GetError() << std::endl;
		exit(1);
	}

	{
		using namespace Sound::Music;
		E_DataID ID{};
		while (ID != E_DataID::E_EnumMax)
		{
			m_pInstance->LoadMusic(ID);
			ID = (E_DataID)(ID + 1);
		}
	}
	{
		using namespace Sound::Chunk::Apple;
		E_DataID ID = (E_DataID)(E_DataID::E_None + 1);
		while (ID != E_DataID::E_EnumMax)
		{
			m_pInstance->LoadChunk(ID);
			ID = (E_DataID)(ID + 1);
		}
	}

	m_pInstance->m_nMusicVolume = 10;
	m_pInstance->m_nChunkVolume = 5;
}

void SoundManager::DestroyInstance()
{
	if (!m_pInstance) 
		return;
	{
		using namespace Sound::Music;
		E_DataID ID{};
		while (ID != E_DataID::E_EnumMax)
		{
			Mix_Music*& music = m_pInstance->m_arMusic[ID];
			Mix_FreeMusic(music);
			music = nullptr;
			ID = (E_DataID)(ID + 1);
		}
	}
	{
		using namespace Sound::Chunk::Apple;
		E_DataID ID{};
		while (ID != E_DataID::E_EnumMax)
		{
			Mix_Chunk*& Chunk = m_pInstance->m_archunk[ID];
			Mix_FreeChunk(Chunk);
			Chunk = nullptr;
			ID = (E_DataID)(ID + 1);
		}
	}
	delete(m_pInstance);
	m_pInstance = nullptr;
	Mix_CloseAudio();
}

std::string SoundManager::GetSoundPath()
{
	return "../Resources/Sound/";
}

std::string SoundManager::GetChunkName(Sound::Chunk::Apple::E_DataID eChunkID)
{
	using namespace Sound::Chunk::Apple;
	std::string arData[E_DataID::E_EnumMax]{};
	arData[E_RedApple] = "RedApple.wav";
	arData[E_GoldApple] = "GoldApple.wav";
	arData[E_BlueApple] = "BlueApple.wav";
	return arData[eChunkID];
}

std::string SoundManager::GetMusicName(Sound::Music::E_DataID eMusicID)
{
	using namespace Sound::Music;
	std::string arData[E_DataID::E_EnumMax]{};
	arData[E_GameOver] = "GameOver.mp3";
	arData[E_Background] = "BackgroundMusic.mp3";
	return arData[eMusicID];
}

bool SoundManager::LoadMusic(Sound::Music::E_DataID eMusicID)
{
	Mix_Music*& music = m_pInstance->m_arMusic[eMusicID];
	if (music)
		return true;
	std::string File = GetSoundPath() + "Music/" + GetMusicName(eMusicID);
	music = Mix_LoadMUS(File.c_str());
	if (music == 0) {
		std::cout << File << Mix_GetError() << std::endl;
		return false;
	}
	return true;
}

bool SoundManager::LoadChunk(Sound::Chunk::Apple::E_DataID eChunkID)
{
	Mix_Chunk*& chunk = m_pInstance->m_archunk[eChunkID];
	if (chunk)
		return true;
	std::string File = GetSoundPath() + "Chunk/" + GetChunkName(eChunkID);
	chunk = Mix_LoadWAV(File.c_str());
	if (chunk == 0) {
		std::cout << File << Mix_GetError() << std::endl;
		return false;
	}
	return true;
}

void SoundManager::PlayChannel(Sound::Chunk::Apple::E_DataID eChunkID, int loops, int channel)
{
	Mix_Chunk*& chunk = m_pInstance->m_archunk[eChunkID];
	if (!chunk)
		return;
	Mix_VolumeChunk(chunk, m_pInstance->m_nChunkVolume);
	Mix_PlayChannel(channel, chunk, loops);
}

void SoundManager::FadeInMusic(Sound::Music::E_DataID eMusicID, int loops, int ms)
{
	Mix_Music*& music = m_pInstance->m_arMusic[eMusicID];
	if (m_pInstance->m_pMainMusic == music || music == nullptr)
		return;
	m_pInstance->m_pMainMusic = music;
	Mix_VolumeMusic(m_pInstance->m_nMusicVolume);
	Mix_FadeInMusic(music, loops, ms);
}

// ¹è°æ À½¾Ç off
void SoundManager::FadeOutMusic(int ms) {
	m_pInstance->m_pMainMusic = nullptr;
	Mix_FadeOutMusic(ms);
}
