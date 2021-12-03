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
#include "misc.h"


using namespace std;


DWORD base5;
DWORD cocos_base5;
HANDLE pHandle5;
vector<bool> turned_on5;
float width5 = 0.0f;
bool need_sort5 = false;
bool need_resize5 = false;
float padding5 = 5.0f;


void Misc::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Misc"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		ImGui::Checkbox("Layout Mode (Bugged Alpha)", &PlayLayer::get_bool_var(7));
		if (need_resize5)
		{
			need_resize5 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width5, io.FontGlobalScale * 200.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width5) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width5, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 200.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 200.0f));
			}
		}
		if (need_sort5)
		{
			need_sort5 = false;
			if (padding5 * 4.0f + width5 * 5.0f * io.FontGlobalScale > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding5 * (float)(smallest_id + 1) + width5 * (float)(smallest_id)*io.FontGlobalScale, last_smallest + padding5));
				h_array.at(smallest_id) += padding5 + io.FontGlobalScale * 200.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding5 * 5.0f + width5 * 4.0f * io.FontGlobalScale, padding5));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 200.0f + padding5);
			}
		}
	}
	ImGui::End();
}


void Misc::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base5 = base_;
	cocos_base5 = cocos_base_;
	pHandle5 = pHandle_;
	width5 = width_;
	padding5 = padding_;
}


void Misc::sort() {
	need_sort5 = true;
}


void Misc::resize() {
	need_resize5 = true;
}
