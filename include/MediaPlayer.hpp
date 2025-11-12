#ifndef MEDIAPLAYER_HPP
#define MEDIAPLAYER_HPP
#include <SFML/Audio.hpp>
#include <vector>
#include <filesystem>

enum SoundEffect {
	MOVE = 0,
	CAPTURE = 1,
	ILLEGAL = 2
};

class MediaPlayer {
public:

	void init();
	void play_background_music();
	void play_audio(SoundEffect cur_song);

	void setMusicVolume(int volume);
	void setAudioVolume(int volume);
private:
	sf::Music background_music;
	std::vector<sf::SoundBuffer> audio_buffer_list;
	std::vector<sf::Sound> audio_list;
};

#endif