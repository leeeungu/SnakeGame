#pragma once

class SoundManager
{
private:
	SoundManager() {};
	static SoundManager* instance_;

public:

	// Singleton
	static void CreateInstance();
	static void DestroyInstance();
	static SoundManager* GetSingleton();

	// Load Sound
	bool LoadMusic(std::string music_id, std::string file_name_mp3);
	bool LoadChunk(std::string chunk_id, std::string file_name_wav, int volume = 5);

	// main music
	void FadeInMusic(std::string music_id, int loops = -1 , int ms = 1000);
	void FadeOutMusic(int ms);

	// channel chunk
	void PlayChannel(std::string chunk_id, int loops = 0, int channel = -1);

	// Setting
	void SetVolumeMusic(int volume);
	void SetVolumeChunk(std::string chunk_id, int volume);

	// Mix_Music* GetMisic(std::string texture_id) { return music_map_[texture_id]; }
private:
	std::string main_Music;
	std::map<std::string, Mix_Music*> music_map_;
	std::map<std::string, Mix_Chunk*> chunk_map_;
};

