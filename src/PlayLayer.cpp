#include <windows.h>
#include <cocos2d.h>
#include <imgui.h>
#include <MinHook.h>
#include <queue>
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <imgui-hook.hpp>
#include <gd.h>
#include <algorithm>
#include <regex>
#include "PlayLayer.h"
#include "mem_utils.h"
#include "main_menu.h"
#include "SpeedhackAudio.h"
#include "sound_system.h"
#include "rand.h"


using namespace std;
using namespace cocos2d;
using namespace std::chrono;


namespace PlayLayer {
	DWORD base;
	DWORD cocos_base;
	HANDLE pHandle;

	float shitty_speedhack = 1.0f;
	bool is_inited = false;
	bool use_my_delta = false;
	bool show_cursor_before = false;
	bool pause_button = false;
	bool show_total_attempts = false;
	bool rgb_attempts_ = false;
	bool enable_fps_bypass = false;
	bool enable_speedhack = false;
	bool enable_speedhack_audio = false;
	bool layout_mode = false;
	bool auto_practice_mode = false;
	float speedhack_speed = 1.0f;
	float speedhack_audio_speed = 1.0f;
	float scale_x_ = 1.0f;
	float scale_y_ = 1.0f;
	float pos_x_ = 0.0f;
	float pos_y_ = 0.0f;
	float skew_x_ = 0.0f;
	float skew_y_ = 0.0f;
	int fake_bypass_plus_ = 1;
	bool hide_att = false;
	float frame_rate_ = 1.0f / 60.0f;
	float default_frame_rate = 1.0f / 60.0f;
	bool speed_up = false;
	float speed_up1 = 0.5f;
	float speed_up2 = 1.5f;
	int player_rainbow = 0;
	float player_rainbow_speed = 1.0f;
	float player_rainbow_speed2 = 1.0f;
	int player2_rainbow = 0;
	float player2_rainbow_speed = 1.0f;
	float player2_rainbow_speed2 = 1.0f;
	int player3_rainbow = 0;
	float player3_rainbow_speed = 1.0f;
	int player4_rainbow = 0;
	float player4_rainbow_speed = 1.0f;
	bool spin_ = false;
	float spin_time = 5.0f;
	bool is_able_to_update = true;
	bool is_first_win = false;
	gd::PlayLayer* play_layer;
	uint64_t last_time;
	string attempt_format("Attempt %i");
	CCAction* attempt_rgb;
	gd::GameObject* bg;
	float rgb_att_speed = 0.5f;
	bool enable_c = true;
	bool last_dead = false;
	bool last_finish = false;
	bool last_finish2 = false;
	bool last_practice = false;
	bool is_left = false;
	bool is_right = false;
	bool is_up = false;
	bool is_down = false;
	bool fix_plat_cube = false;
	bool enable_death_sound = false;
	float last_speed = 1.0f;
	float respawn_time = 1.0f;
	DWORD speed_addr;
	DWORD speed2_addr;
	DWORD ball_addr;
	bool is_platformer = false;
	int deaths = 0;
	int deaths1 = 0;
	bool enable_hleb = false;
	time_t last_sec = 0;
	CCSprite* hleb1;
	CCSprite* hleb2;

	ccColor3B black_ = { 0, 0, 0 };
	ccColor3B bg_ = { 40, 125, 255 };
	ccColor3B g1_ = { 0, 102, 255 };
	ccColor3B white_ = { 255, 255, 255 };

	CCAction* create_rgb(float speed, bool is_reverse = false) {
		return is_reverse ?
			CCRepeatForever::create(CCSequence::create(
				CCTintTo::create(speed, 0, 255, 255),
				CCTintTo::create(speed, 0, 0, 255),
				CCTintTo::create(speed, 255, 0, 255),
				CCTintTo::create(speed, 255, 0, 0),
				CCTintTo::create(speed, 255, 255, 0),
				CCTintTo::create(speed, 0, 255, 0),
				nullptr
			)) : CCRepeatForever::create(CCSequence::create(
				CCTintTo::create(speed, 255, 0, 0),
				CCTintTo::create(speed, 255, 255, 0),
				CCTintTo::create(speed, 0, 255, 0),
				CCTintTo::create(speed, 0, 255, 255),
				CCTintTo::create(speed, 0, 0, 255),
				CCTintTo::create(speed, 255, 0, 255),
				nullptr
			));
	}

	void update_speedhack(bool a1, bool a2, float a3, float a4) {
		enable_speedhack = a1;
		enable_speedhack_audio = a2;
		speedhack_speed = a3;
		speedhack_audio_speed = a4;
		if (a1 && is_inited) {
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(a3);
		}
		else {
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
		}
		if (a2 && is_inited) {
			SpeedhackAudio::set(a4);
		}
		else {
			SpeedhackAudio::set(1.0f);
		}
	}
	void set_attempt_format(char* format) {
		attempt_format = string(format);
	}
	void update_rainbow(int idx, int type, float speed, float speed2 = 1.0f) {
		if (idx == 0) {
			player_rainbow = type;
			player_rainbow_speed = speed;
			player_rainbow_speed2 = speed2;
			return;
		}
		if (idx == 1) {
			player2_rainbow = type;
			player2_rainbow_speed = speed;
			player2_rainbow_speed2 = speed2;
			return;
		}
		if (idx == 2) {
			player3_rainbow = type;
			player3_rainbow_speed = speed;
			return;
		}
		if (idx == 3) {
			player4_rainbow = type;
			player4_rainbow_speed = speed;
			return;
		}
	}
	int& get_int_var(int idx) {
		if (idx == 0)
			return fake_bypass_plus_;
		return fake_bypass_plus_;
	}
	float& get_float_var(int idx) {
		if (idx == 0)
			return shitty_speedhack;
		if (idx == 1)
			return rgb_att_speed;
		if (idx == 2)
			return frame_rate_;
		if (idx == 3)
			return default_frame_rate;
		if (idx == 4)
			return speed_up1;
		if (idx == 5)
			return speed_up2;
		if (idx == 6)
			return spin_time;
		if (idx == 7)
			return scale_x_;
		if (idx == 8)
			return scale_y_;
		if (idx == 9)
			return pos_x_;
		if (idx == 10)
			return pos_y_;
		if (idx == 11)
			return skew_x_;
		if (idx == 12)
			return skew_y_;
		if (idx == 13)
			return respawn_time;
		return shitty_speedhack;
	}
	bool& get_bool_var(int idx) {
		bool always_false = false;

		if (idx == 0)
			return is_inited;
		if (idx == 1)
			return use_my_delta;
		if (idx == 2)
			return pause_button;
		if (idx == 3)
			return show_total_attempts;
		if (idx == 4)
			return rgb_attempts_;
		if (idx == 5)
			return hide_att;
		if (idx == 6)
			return enable_fps_bypass;
		if (idx == 7)
			return layout_mode;
		if (idx == 9)
			return auto_practice_mode;
		if (idx == 10)
			return speed_up;
		if (idx == 11)
			return spin_;
		if (idx == 12)
			return is_first_win;
		if (idx == 17)
			return is_platformer;
		if (idx == 18)
			return fix_plat_cube;
		if (idx == 19)
			return enable_hleb;
		if (is_inited) {
			if (idx == 8)
				return play_layer->m_isPracticeMode;
			if (idx == 13)
				return is_left;
			if (idx == 14)
				return is_right;
			if (idx == 15)
				return is_up;
			if (idx == 16)
				return is_down;
		}
		else {

		}
		return always_false;
	}

	bool __fastcall PlayLayer::pushButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton) {
		if (!is_inited)
			return PlayLayer::pushButton(self, PlayerButton);
		bool is_first_player = self->m_pBaseColor == play_layer->m_pPlayer1->m_pBaseColor;
		bool is_second_player = self->m_pBaseColor == play_layer->m_pPlayer2->m_pBaseColor;
		if (is_first_player && enable_hleb) {
			SoundSystem::play_batya_sound();
			hleb1->setVisible(false);
			hleb2->setVisible(true);
		}
		return PlayLayer::pushButton(self, PlayerButton);
	}

	bool __fastcall PlayLayer::releaseButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton) {
		if (!is_inited)
			return PlayLayer::releaseButton(self, PlayerButton);
		bool is_first_player = self->m_pBaseColor == play_layer->m_pPlayer1->m_pBaseColor;
		bool is_second_player = self->m_pBaseColor == play_layer->m_pPlayer2->m_pBaseColor;
		if (is_first_player && enable_hleb) {
			hleb1->setVisible(true);
			hleb2->setVisible(false);
		}
		return PlayLayer::releaseButton(self, PlayerButton);
	}

	void __fastcall PlayLayer::resetLevelHook(gd::PlayLayer* self, void*) {
		last_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		enable_c = !layout_mode;
		PlayLayer::resetLevel(self);
		enable_c = true;
		self->m_attemptLabel->setCString((regex_replace(attempt_format, regex("\\%i"), to_string(show_total_attempts ? self->m_level->m_nAttempts : self->m_currentAttempt))).c_str());
		if (attempt_rgb != nullptr) {
			self->m_attemptLabel->stopAction(attempt_rgb);
		}
		if (rgb_attempts_) {
			attempt_rgb = create_rgb(rgb_att_speed);
			self->m_attemptLabel->runAction(attempt_rgb);
		}

		last_finish = false;
		last_finish2 = false;
		last_dead = false;

		if (enable_speedhack) {
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(speedhack_speed);
		}
		else {
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
		}
		if (enable_speedhack_audio) {
			SpeedhackAudio::set(speedhack_audio_speed);
		}
		else {
			SpeedhackAudio::set(1.0f);
		}

		self->m_attemptLabel->setVisible(!hide_att || !self->m_isPracticeMode);

		SoundSystem::stop_sound_playing();
		CCApplication::sharedApplication()->setAnimationInterval(enable_fps_bypass ? frame_rate_ : default_frame_rate);
	}

	void __fastcall PlayLayer::onQuitHook(gd::PlayLayer* self, void*) {
		is_inited = false;

		gd::GameManager* gm = gd::GameManager::sharedState();
		gm->setIntGameVariable("0024", (int)show_cursor_before);

		SpeedhackAudio::set(1.0f);
		CCApplication::sharedApplication()->setAnimationInterval(default_frame_rate);
		CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
		SoundSystem::unload_death_sounds();

		MyRandom::random_all();

		PlayLayer::onQuit(self);
	}

	bool __fastcall PlayLayer::initHook(gd::PlayLayer* self, void*, void* level) {
		gd::GameManager* gm = gd::GameManager::sharedState();
		show_cursor_before = (bool)gm->getIntGameVariable("0024");

		gm->setIntGameVariable("0024", 1);

		bool result = PlayLayer::init(self, level);
		play_layer = self;

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		CCNode* pause_button1 = (CCNode*)self->m_uiLayer->getChildren()->objectAtIndex(0);
		if (!pause_button)
			pause_button1->setPositionX(-999.0f);

		CCApplication::sharedApplication()->setAnimationInterval(enable_fps_bypass ? frame_rate_ : default_frame_rate);
		if (enable_speedhack) {
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(speedhack_speed);
		}
		if (enable_speedhack_audio) {
			SpeedhackAudio::set(speedhack_audio_speed);
		}
		else {
			SpeedhackAudio::set(1.0f);
		}

		if (player_rainbow == 1) {
			CCAction* rgb1 = create_rgb(player_rainbow_speed);
			CCAction* rgb2 = create_rgb(player_rainbow_speed, true);
			((CCNode*)self->m_pPlayer1)->runAction((CCAction*)rgb1->copy());
			((CCNode*)((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb2->copy());
			//((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(1))->runAction((CCAction*)rgb1->copy());
			((CCNode*)((CCNode*)self->m_pPlayer1->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb2->copy());
		}
		if (player2_rainbow == 1) {
			CCAction* rgb1 = create_rgb(player2_rainbow_speed);
			CCAction* rgb2 = create_rgb(player2_rainbow_speed, true);
			((CCNode*)self->m_pPlayer2)->runAction((CCAction*)rgb2->copy());
			((CCNode*)((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb1->copy());
			//((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(1))->runAction((CCAction*)rgb2->copy());
			((CCNode*)((CCNode*)self->m_pPlayer2->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->runAction((CCAction*)rgb1->copy());
		}
		if (player3_rainbow) {
			((CCNode*)self->m_pObjectLayer->getChildren()->objectAtIndex(49))->runAction(create_rgb(player3_rainbow_speed));
		}
		if (player4_rainbow) {
			((CCNode*)self->m_pObjectLayer->getChildren()->objectAtIndex(50))->runAction(create_rgb(player4_rainbow_speed));
		}

		for (int i = 0; i < self->getChildrenCount(); i++) {
			CCSprite* child = (CCSprite*)self->getChildren()->objectAtIndex(i);
			if (child->getZOrder() == -1) {
				bg = (gd::GameObject*)child;
				break;
			}
		}
		if (auto_practice_mode) {
			self->togglePracticeMode(true);
		}
		if (spin_) {
			self->runAction(CCRepeatForever::create(CCSequence::create(
				CCRotateTo::create(spin_time / 2.0f, 180.0f),
				CCRotateTo::create(spin_time / 2.0f, 360.0f),
				nullptr
			)));
		}

		deaths1 = 0;
		last_practice = false;
		speed_addr = MemUtils::get_offset({ 0x3222D0, 0x164, 0x224, 0x648 });
		speed2_addr = (DWORD)self->m_pPlayer2 + 0x648;
		ball_addr = (DWORD)self->m_pPlayer1 + 0x63A;

		hleb1 = CCSprite::create("GDOC\\za_hlebom1.png");
		hleb2 = CCSprite::create("GDOC\\za_hlebom2.png");
		if (enable_hleb) {
			hleb1->setAnchorPoint({ 0, 0 });
			hleb1->setPositionY(winSize.height - hleb1->getContentSize().height);
			hleb1->setScale(1.0f);
			hleb1->setTag(4564353);
			hleb2->setAnchorPoint({ 0, 0 });
			hleb2->setPositionY(winSize.height - hleb2->getContentSize().height);
			hleb2->setScale(1.0f);
			hleb2->setTag(4256356);
			hleb2->setVisible(false);
			self->addChild(hleb1, 9999);
			self->addChild(hleb2, 9999);
		}

		last_sec = time(0);
		last_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		MyRandom::random_all();
		SoundSystem::load_death_sounds();

		is_inited = true;
		return result;
	}

	void __fastcall PlayLayer::updateHook(gd::PlayLayer* self, void* ff, float dt) {
		if (!is_inited)
			return PlayLayer::update(self, dt);
		if (is_able_to_update && fake_bypass_plus_ > 1) {
			is_able_to_update = false;
			for (int i = 0; i < fake_bypass_plus_; i++) {
				PlayLayer::updateHook(self, 0, dt / (float)fake_bypass_plus_);
			}
			is_able_to_update = true;
			return;
		}
		uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		time_t now_sec = time(0);
		float player_x = self->m_pPlayer1->getPositionX();
		float player_y = self->m_pPlayer1->getPositionY();
		if (speed_up) {
			if (enable_speedhack) {
				speedhack_speed = (speed_up2 - speed_up1) * (player_x / self->m_levelLength) + speed_up1;
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(speedhack_speed);
			}
			if (enable_speedhack_audio) {
				speedhack_audio_speed = speedhack_speed;
				SpeedhackAudio::set(speedhack_audio_speed);
			}
		}
		float delta = (now - last_time) * (enable_speedhack ? speedhack_speed : 1.0f) / 1000.0f;
		last_time = now;
		float fps = (enable_speedhack ? speedhack_speed : 1.0f) / (use_my_delta ? delta : dt);
		if (self->m_isDead) {
			if (!last_dead) {
				if (enable_death_sound)
					SoundSystem::play_random_death_sound();
				last_dead = true;
				deaths += 1;
				deaths1 += 1;
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f / respawn_time);
			}
		}
		if (self->m_isPracticeMode) {
			if (!last_practice) {
				last_practice = true;
				deaths1 = 0;
			}
		}
		else if (last_practice) {
			last_practice = false;
		}
		if (self->m_hasCompletedLevel && !last_finish) {
			last_finish = true;
			if (is_first_win && self->m_isPracticeMode && deaths1 == 0) {
				self->m_isPracticeMode = false;
			}
		}
		if (self->m_hasLevelCompleteMenu && !last_finish2) {
			last_finish2 = true;
		}
		if (is_platformer) {
			float speed_;
			ReadProcessMemory(pHandle, reinterpret_cast<void*>(speed_addr), &speed_, sizeof(speed_), NULL);
			if (speed_ > 0.0f) {
				last_speed = speed_;
			}
			if (is_right) {
				speed_ = last_speed;
			}
			else if (is_left) {
				speed_ = -last_speed;
			}
			else {
				speed_ = 0.0f;
			}
			WriteProcessMemory(pHandle, reinterpret_cast<void*>(speed_addr), &speed_, sizeof(speed_), NULL);
			WriteProcessMemory(pHandle, reinterpret_cast<void*>(speed2_addr), &speed_, sizeof(speed_), NULL);
		}
		if (fix_plat_cube) {
			bool is_ball;
			ReadProcessMemory(pHandle, reinterpret_cast<void*>(ball_addr), &is_ball, sizeof(is_ball), NULL);
			if (is_ball) {
				MemUtils::write_bytes(base + 0x1E9BF0, { 0x57, 0x8B, 0xF9 });
			}
			else {
				MemUtils::write_bytes(base + 0x1E9BF0, { 0xC2, 0x04, 0x00 });
			}
		}
		if (layout_mode) {
			for (int i = 0; i < self->m_pObjects->count(); i++) {
				gd::GameObject* obj = (gd::GameObject*)self->m_pObjects->objectAtIndex(i);
				if (obj->m_nObjectType == gd::kGameObjectTypeDecoration) {
					obj->removeMeAndCleanup();
					continue;
				};
				obj->setVisible(true);
				obj->setOpacity(255);
				obj->setColor(white_);
			}
			bg->stopAllActions();
			bg->setColor(bg_);
			enable_c = false;
		}

		self->setScaleX(scale_x_);
		self->setScaleY(scale_y_);
		self->setPositionX(pos_x_);
		self->setPositionY(pos_y_);
		self->setSkewX(skew_x_);
		self->setSkewY(skew_y_);
		if (now_sec > last_sec) {
			last_sec = now_sec;
		}
		CCApplication::sharedApplication()->setAnimationInterval(enable_fps_bypass ? frame_rate_ : default_frame_rate);
		PlayLayer::update(self, (use_my_delta ? delta : dt) * shitty_speedhack);
		CCApplication::sharedApplication()->setAnimationInterval(enable_fps_bypass ? frame_rate_ : default_frame_rate);
		enable_c = true;
	}

	inline void(__thiscall* setColorO)(CCSprite* self, const ccColor3B& color);
	void __fastcall setColorH(CCSprite* self, void*, const ccColor3B& color) {
		if (enable_c) {
			setColorO(self, color);
		}
		else {
			//setColorO(self, white_);
		}
	}

	void PlayLayer::mem_init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_) {
		base = base_;
		cocos_base = cocos_base_;
		pHandle = pHandle_;

		DWORD addr = (DWORD)GetProcAddress(GetModuleHandle("libcocos2d.dll"), "?setColor@CCSprite@cocos2d@@UAEXABU_ccColor3B@2@@Z");

		MH_CreateHook(
			(PVOID)(base + 0x2029C0),
			PlayLayer::updateHook,
			(PVOID*)&PlayLayer::update
		);

		MH_CreateHook(
			(PVOID)(base + 0x20BF00),
			PlayLayer::resetLevelHook,
			(PVOID*)&PlayLayer::resetLevel
		);

		MH_CreateHook(
			(PVOID)(base + 0x20D810),
			PlayLayer::onQuitHook,
			(PVOID*)&PlayLayer::onQuit
		);

		MH_CreateHook(
			(PVOID)(base + 0x01FB780),
			PlayLayer::initHook,
			(PVOID*)&PlayLayer::init
		);

		MH_CreateHook(
			(PVOID)(addr),
			setColorH,
			(PVOID*)&setColorO
		);

		MH_CreateHook(
			(PVOID)(base + 0x1F4E40),
			PlayLayer::pushButtonHook,
			(LPVOID*)&PlayLayer::pushButton
		);

		MH_CreateHook(
			(PVOID)(base + 0x1F4F70),
			PlayLayer::releaseButtonHook,
			(LPVOID*)&PlayLayer::releaseButton
		);
	}
}