#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <mmsystem.h>
#include <filesystem>
#include <vector>
#include "sound_system.h"
#include "rand.h"
#pragma comment(lib, "Winmm.lib")


using namespace std;


namespace SoundSystem {
	vector<string> sounds;
	int current_id = -1;

	void play_batya_sound() {
		PlaySound("GDOC\\gd_hleb_sound.wav", NULL, SND_ASYNC);
	}
	void load_death_sounds() {
		sounds.clear();
		current_id = -1;
		for (const auto& entry : std::filesystem::directory_iterator("GDOC\\death_sounds")) {
			string path = entry.path().relative_path().string();
			sounds.push_back(path);
			mciSendString(
				("open \"" + path + "\" alias \"" + path + "\"").c_str(),
				NULL,
				NULL, 
				NULL
			);
		}
	}
	void play_random_death_sound() {
		if (sounds.size() <= 0)
			return;
		stop_sound_playing();
		current_id = MyRandom::random_int(0, sounds.size() - 1);
		mciSendString(
			("play \"" + sounds.at(current_id) + "\"").c_str(),
			NULL,
			NULL,
			NULL
		);
	}
	void unload_death_sounds() {
		stop_sound_playing();
		for (int i = 0; i < sounds.size(); i++) {
			mciSendString(
				("close \"" + sounds.at(i) + "\"").c_str(),
				NULL,
				NULL,
				NULL
			);
		}
		current_id = -1;
		sounds.clear();
	}
	void stop_sound_playing() {
		if (current_id < 0)
			return;
		mciSendString(
			("stop \"" + sounds.at(current_id) + "\"").c_str(),
			NULL,
			NULL,
			NULL
		);
		mciSendString(
			("seek \"" + sounds.at(current_id) + "\" to start").c_str(),
			NULL,
			NULL,
			NULL
		);
		current_id = -1;
	}
}
