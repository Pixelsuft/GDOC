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
#include "mods_menu.h"


using namespace std;


DWORD base10;
DWORD cocos_base10;
HANDLE pHandle10;
vector<bool> turned_on10;
float width10 = 0.0f;
bool need_sort10 = false;
bool need_resize10 = false;
float padding10 = 5.0f;


void ModsMenu::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size, ImGuiStyle& style) {
	if (ImGui::Begin("Mods/Fun"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		if (ImGui::TreeNode("Speed Up")) {
			ImGui::Text("Make Sure That You");
			ImGui::Text("Have Enabled Speedhack");
			ImGui::Checkbox("Use", &PlayLayer::get_bool_var(10));
			ImGui::DragFloat("0%", &PlayLayer::get_float_var(4), 0.05f, 0.01f, 999.0f, "%f", 0.0f);
			ImGui::DragFloat("100%", &PlayLayer::get_float_var(5), 0.05f, 0.01f, 999.0f, "%f", 0.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Spin Mode")) {
			ImGui::Checkbox("Use", &PlayLayer::get_bool_var(11));
			ImGui::DragFloat("Time", &PlayLayer::get_float_var(6), 0.05f, 0.01f, 999.0f, "%f", 0.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Play Layer")) {
			ImGui::Text("Scale");
			ImGui::DragFloat("Xs", &PlayLayer::get_float_var(7), 0.05f, -999.0f, 999.0f, "%f", 0.0f);
			ImGui::DragFloat("Ys", &PlayLayer::get_float_var(8), 0.05f, -999.0f, 999.0f, "%f", 0.0f);
			ImGui::Text("Position");
			ImGui::DragFloat("Xp", &PlayLayer::get_float_var(9), 5.0f, -9999.0f, 9999.0f, "%f", 0.0f);
			ImGui::DragFloat("Yp", &PlayLayer::get_float_var(10), 5.0f, -9999.0f, 9999.0f, "%f", 0.0f);
			ImGui::Text("Skew");
			ImGui::DragFloat("Xr", &PlayLayer::get_float_var(11), 0.5f, -360.0f, 360.0f, "%f", 0.0f);
			ImGui::DragFloat("Yr", &PlayLayer::get_float_var(12), 0.5f, -360.0f, 360.0f, "%f", 0.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Platformer")) {
			ImGui::Checkbox("Platformer", &PlayLayer::get_bool_var(17));
			ImGui::Checkbox("Fix Cube Rotation", &PlayLayer::get_bool_var(18));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("GD Hleb Mod")) {
			ImGui::Checkbox("Use", &PlayLayer::get_bool_var(19));
			ImGui::TreePop();
		}

		if (need_resize10)
		{
			need_resize10 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width10, io.FontGlobalScale * 400.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width10) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width10, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 400.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 400.0f));
			}
		}
		if (need_sort10)
		{
			need_sort10 = false;
			if (padding10 * 10.0f + width10 * 10.0f * io.FontGlobalScale > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding10 * (float)(smallest_id + 1) + width10 * (float)(smallest_id)*io.FontGlobalScale, last_smallest + padding10));
				h_array.at(smallest_id) += padding10 + io.FontGlobalScale * 400.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding10 * 10.0f + width10 * 9.0f * io.FontGlobalScale, padding10));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 400.0f + padding10);
			}
		}
	}
	ImGui::End();
}


void ModsMenu::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base10 = base_;
	cocos_base10 = cocos_base_;
	pHandle10 = pHandle_;
	width10 = width_;
	padding10 = padding_;
}


void ModsMenu::sort() {
	need_sort10 = true;
}


void ModsMenu::resize() {
	need_resize10 = true;
}
