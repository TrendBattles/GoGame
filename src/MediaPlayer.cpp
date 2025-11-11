#include <MediaPlayer.hpp>
#include <iostream>
#include <vector>

void MediaPlayer::init() {
	const std::filesystem::path BACKGROUND_SONG =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/GameOST.ogg");
	if (!background_music.openFromFile(BACKGROUND_SONG.c_str())) {
		exit(1);
	}
	background_music.setVolume(40);


	const std::filesystem::path MOVE_SOUND =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Move.ogg");
	const std::filesystem::path CAPTURE_SOUND =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/Capture.ogg");
	const std::filesystem::path ILLEGAL_SOUND =
		std::filesystem::absolute(std::string(PROJECT_DIR) + "assets/IllegalMove.ogg");

	audio_buffer_list.emplace_back(sf::SoundBuffer(MOVE_SOUND.c_str()));
	audio_buffer_list.emplace_back(sf::SoundBuffer(CAPTURE_SOUND.c_str()));
	audio_buffer_list.emplace_back(sf::SoundBuffer(ILLEGAL_SOUND.c_str()));

	
	for (auto &i : audio_buffer_list) {
		audio_list.emplace_back(sf::Sound(i));
	}
}

void MediaPlayer::play_background_music() {
	background_music.setLooping(true);
	background_music.play();
}

void MediaPlayer::play_audio(SoundEffect current_sound) {
	audio_list[int(current_sound)].play();
}