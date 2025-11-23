#include <MediaPlayer.hpp>
#include <iostream>
#include <vector>

void MediaPlayer::init() {
	setAudioType(1);
}

void MediaPlayer::play_background_music() {
	background_music.stop();
	background_music.setLooping(true);
	background_music.play();
}

void MediaPlayer::play_audio(SoundEffect current_sound) {
	audio_list[int(current_sound)].play();
}

void MediaPlayer::setMusicVolume(int vol) {
	background_music.setVolume(vol);
}


void MediaPlayer::setAudioVolume(int vol) {
	for (auto& i : audio_list) 
		i.setVolume(vol);
}

std::string to_string(int x) {
	if (x == 0) return "0";
	std::string res;
	while (x > 0) {
		res.push_back('0' + x % 10);
		x /= 10;
	}
	reverse(res.begin(), res.end());
	return res;
}

bool MediaPlayer::setAudioType(int t) {
	if (current_type == t) return false;

	current_type = t;
	audio_buffer_list.clear();
	audio_list.clear();

	const std::string PREFIX = std::string(PROJECT_DIR) + "assets/Sound/Pack" + to_string(t) + "/";

	const std::filesystem::path BACKGROUND_SONG =
		std::filesystem::absolute(PREFIX + "GameOST.ogg");
	if (!background_music.openFromFile(BACKGROUND_SONG.c_str())) {
		exit(1);
	}


	const std::filesystem::path MOVE_SOUND =
		std::filesystem::absolute(PREFIX + "Move.ogg");
	const std::filesystem::path CAPTURE_SOUND =
		std::filesystem::absolute(PREFIX + "Capture.ogg");
	const std::filesystem::path ILLEGAL_SOUND =
		std::filesystem::absolute(PREFIX + "IllegalMove.ogg");
	const std::filesystem::path CLICK_SOUND =
		std::filesystem::absolute(PREFIX + "Click.ogg");

	audio_buffer_list.emplace_back(sf::SoundBuffer(MOVE_SOUND.c_str()));
	audio_buffer_list.emplace_back(sf::SoundBuffer(CAPTURE_SOUND.c_str()));
	audio_buffer_list.emplace_back(sf::SoundBuffer(ILLEGAL_SOUND.c_str()));
	audio_buffer_list.emplace_back(sf::SoundBuffer(CLICK_SOUND.c_str()));

	for (auto& i : audio_buffer_list) {
		audio_list.emplace_back(sf::Sound(i));
	}

	return true;
}