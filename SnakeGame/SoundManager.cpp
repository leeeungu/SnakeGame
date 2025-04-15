#include "stdafx.h"
#include "SoundManager.h"

SoundManager* SoundManager::instance_ = 0;

void SoundManager::CreateInstance() {
	if (instance_ != 0) return;
	instance_ = new SoundManager();
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		std::cout << "Mix_OpenAudio " << Mix_GetError() << std::endl;
		exit(1);
	}
	instance_->SetVolumeMusic(5);
}

void SoundManager::DestroyInstance() {
	if (instance_ == 0) return;
	{
		std::map<std::string, Mix_Music*>::iterator iter = instance_->music_map_.begin();
		while (iter != instance_->music_map_.end())
		{
			Mix_FreeMusic(iter->second);
			iter++;
		}
		instance_->music_map_.clear();
	}
	{
		std::map<std::string, Mix_Chunk*>::iterator iterChunk = instance_->chunk_map_.begin();
		while (iterChunk != instance_->chunk_map_.end())
		{
			Mix_FreeChunk(iterChunk->second);
			iterChunk++;
		}
		instance_->chunk_map_.clear();
	}
	delete(instance_);
	Mix_CloseAudio();
}

SoundManager* SoundManager::GetSingleton() {
	return instance_;
}


// 배경 음악등을 load 하는 함수
bool SoundManager::LoadMusic(std::string music_id, std::string file_name_mp3) {
	if (instance_->music_map_[music_id] != nullptr)
		return true;

	Mix_Music* music = Mix_LoadMUS(file_name_mp3.c_str());
	if (music == 0) {
		std::cout << file_name_mp3 << Mix_GetError() << std::endl;
		return false;
	}
	instance_->music_map_[music_id] = music;
	return true;
}

// 총소리 처럼 단기적인 음악을 load 하는 함수
bool SoundManager::LoadChunk(std::string chunk_id, std::string file_name_wav , int volume ) {
	if (instance_->chunk_map_[chunk_id] != nullptr)
		return true;

	Mix_Chunk* chunk = Mix_LoadWAV(file_name_wav.c_str());
	if (chunk == 0) {
		std::cout << file_name_wav << Mix_GetError() << std::endl;
		return false;
	}
	instance_->chunk_map_[chunk_id] = chunk;
	Mix_VolumeChunk(instance_->chunk_map_[chunk_id], volume);
	return true;
}

void SoundManager::PlayChannel(std::string Channel_id, int loops, int channel) {
	Mix_PlayChannel(channel, instance_->chunk_map_[Channel_id], loops);
}

void SoundManager::SetVolumeMusic(int volume)
{
	 Mix_VolumeMusic(volume);	
}

// 볼륨 조절
void SoundManager::SetVolumeChunk(std::string chunk_id, int volume)
{
	Mix_VolumeChunk(instance_->chunk_map_[chunk_id], volume);
}

// 배경 음악 on
void SoundManager::FadeInMusic(std::string music_id, int loops, int ms) {
	if (main_Music == music_id || instance_->music_map_[music_id] == nullptr)
		return;
	Mix_FadeInMusic(instance_->music_map_[music_id], loops, ms);
}

// 배경 음악 off
void SoundManager::FadeOutMusic(int ms) {
	main_Music = "";
	Mix_FadeOutMusic(ms);
}
