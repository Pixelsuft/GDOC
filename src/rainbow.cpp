#include <windows.h>
#include <cocos2d.h>
#include <imgui.h>
#include <MinHook.h>
#include <queue>
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>
#include <imgui-hook.hpp>
#include <gd.h>
#include <vector>
#include "main_menu.h"
#include "mem_utils.h"
#include "PlayLayer.h"
#include "rainbow.h"


using namespace std;


DWORD base8;
DWORD cocos_base8;
HANDLE pHandle8;
vector<bool> turned_on8;
float width8 = 0.0f;
bool need_sort8 = false;
bool need_resize8 = false;
float padding8 = 5.0f;
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


void Rainbow::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Player/Rainbow"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		if (ImGui::TreeNode("Player 1")) {
			if (ImGui::RadioButton("Off", &player_rainbow, 0)) {
				PlayLayer::update_rainbow(0, player_rainbow, player_rainbow_speed, player_rainbow_speed2);
			} ImGui::SameLine();
			if (ImGui::RadioButton("RGB", &player_rainbow, 1)) {
				PlayLayer::update_rainbow(0, player_rainbow, player_rainbow_speed, player_rainbow_speed2);
			}
			if (ImGui::DragFloat("Time", &player_rainbow_speed, 0.5f, 0.01f, 500.0f, "%f", 0)) {
				PlayLayer::update_rainbow(0, player_rainbow, player_rainbow_speed, player_rainbow_speed2);
			}
			if (ImGui::DragFloat("Time2", &player_rainbow_speed2, 0.5f, 0.01f, 500.0f, "%f", 0)) {
				PlayLayer::update_rainbow(0, player_rainbow, player_rainbow_speed, player_rainbow_speed2);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Player 2")) {
			if (ImGui::RadioButton("Off", &player2_rainbow, 0)) {
				PlayLayer::update_rainbow(1, player2_rainbow, player2_rainbow_speed, player2_rainbow_speed2);
			} ImGui::SameLine();
			if (ImGui::RadioButton("RGB", &player2_rainbow, 1)) {
				PlayLayer::update_rainbow(1, player2_rainbow, player2_rainbow_speed, player2_rainbow_speed2);
			}
			if (ImGui::DragFloat("Time", &player2_rainbow_speed, 0.5f, 0.01f, 500.0f, "%f", 0)) {
				PlayLayer::update_rainbow(1, player2_rainbow, player2_rainbow_speed, player2_rainbow_speed2);
			}
			if (ImGui::DragFloat("Time2", &player2_rainbow_speed2, 0.5f, 0.01f, 500.0f, "%f", 0)) {
				PlayLayer::update_rainbow(1, player2_rainbow, player2_rainbow_speed, player2_rainbow_speed2);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Trail")) {
			if (ImGui::RadioButton("Off", &player3_rainbow, 0)) {
				PlayLayer::update_rainbow(2, player3_rainbow, player3_rainbow_speed, 0.0f);
			} ImGui::SameLine();
			if (ImGui::RadioButton("RGB", &player3_rainbow, 1)) {
				PlayLayer::update_rainbow(2, player3_rainbow, player3_rainbow_speed, 0.0f);
			}
			if (ImGui::DragFloat("Time", &player2_rainbow_speed, 0.5f, 0.01f, 500.0f, "%f", 0)) {
				PlayLayer::update_rainbow(1, player3_rainbow, player3_rainbow_speed, 0.0f);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Wave Trail")) {
			if (ImGui::RadioButton("Off", &player4_rainbow, 0)) {
				PlayLayer::update_rainbow(3, player4_rainbow, player4_rainbow_speed, 0.0f);
			} ImGui::SameLine();
			if (ImGui::RadioButton("RGB", &player4_rainbow, 1)) {
				PlayLayer::update_rainbow(3, player4_rainbow, player4_rainbow_speed, 0.0f);
			}
			if (ImGui::DragFloat("Time", &player4_rainbow_speed, 0.5f, 0.01f, 500.0f, "%f", 0)) {
				PlayLayer::update_rainbow(3, player4_rainbow, player4_rainbow_speed, 0.0f);
			}
			ImGui::TreePop();
		}
		if (need_resize8)
		{
			need_resize8 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width8, io.FontGlobalScale * 200.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width8) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width8, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 200.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 200.0f));
			}
		}
		if (need_sort8)
		{
			need_sort8 = false;
			if (padding8 * 8.0f + width8 * 8.0f * io.FontGlobalScale > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding8 * (float)(smallest_id + 1) + width8 * (float)(smallest_id)*io.FontGlobalScale, last_smallest + padding8));
				h_array.at(smallest_id) += padding8 + io.FontGlobalScale * 200.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding8 * 8.0f + width8 * 7.0f * io.FontGlobalScale, padding8));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 200.0f + padding8);
			}
		}
	}
	ImGui::End();
}


void Rainbow::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base8 = base_;
	cocos_base8 = cocos_base_;
	pHandle8 = pHandle_;
	width8 = width_;
	padding8 = padding_;
}


void Rainbow::sort() {
	need_sort8 = true;
}


void Rainbow::resize() {
	need_resize8 = true;
}
