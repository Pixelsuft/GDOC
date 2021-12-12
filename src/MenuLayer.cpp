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
#include "MenuLayer.h"
#include "player_json_def.h"
#include "main_menu.h"


using namespace std;
using namespace cocos2d;


namespace MenuLayer {
	DWORD base;
	DWORD cocos_base;
	HANDLE pHandle;
	

	bool hide_mg = false;
	bool inited = false;


	bool& get_mg_btn() {
		return hide_mg;
	}


	int __fastcall MenuLayer::initHook(gd::MenuLayer* self) {
		int result = MenuLayer::init(self);

		if (!inited) {
			inited = true;
			MainMenu::get_b(2) = true;
		}

		if (hide_mg) {
			CCNode* mg_btn = (CCNode*)((CCNode*)self->getChildren()->objectAtIndex(5))->getChildren()->objectAtIndex(0);
			mg_btn->setVisible(false);
		}
		
		CCSize win_size = CCDirector::sharedDirector()->getWinSize();
		CCLabelBMFont* watermark = CCLabelBMFont::create("GDOC Beta", "gjFont05.fnt");
		watermark->setTag(1337);
		watermark->setOpacity(GLubyte(100.0f));
		watermark->setPositionX(win_size.width / 2.0f);
		watermark->setPositionY(watermark->getContentSize().height / 2.0f);
		self->addChild(watermark, 0);

		PlayerJson::update_var_player();

		return result;
	}


	void MenuLayer::mem_init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_) {
		base = base_;
		cocos_base = cocos_base_;
		pHandle = pHandle_;

		MH_CreateHook(
			(PVOID)(base + 0x1907B0),
			MenuLayer::initHook,
			(PVOID*)&MenuLayer::init
		);
	}
}
