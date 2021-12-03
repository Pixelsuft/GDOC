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
#include "label.h"
#include "PlayLayer.h"


using namespace std;


DWORD base7;
DWORD cocos_base7;
HANDLE pHandle7;
vector<bool> turned_on7;
float width7 = 0.0f;
bool need_sort7 = false;
bool need_resize7 = false;
bool show_total_attemps = false;
bool rgb_attempts = false;
float att_rgb_speed = 0.5f;
bool hide_att_ = false;
float padding7 = 5.0f;
char attempt_input[256] = "Attempt %i";


void Label::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Label"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		if (ImGui::TreeNode("Attempt Label")) {
			if (ImGui::InputText("Text", attempt_input, sizeof(attempt_input))) {
				PlayLayer::set_attempt_format(attempt_input);
			}
			if (ImGui::Checkbox("Show Total", &show_total_attemps)) {
				PlayLayer::get_bool_var(3) = show_total_attemps;
			}
			if (ImGui::Checkbox("RGB", &rgb_attempts)) {
				PlayLayer::get_bool_var(4) = rgb_attempts;
			}
			if (ImGui::DragFloat("Time", &att_rgb_speed, 0.05f, 0.01f, 50.0f, "%f", 0.0f)) {
				PlayLayer::get_float_var(1) = att_rgb_speed;
			}
			if (ImGui::Checkbox("Hide In Practice Mode", &hide_att_)) {
				PlayLayer::get_bool_var(5) = hide_att_;
			}
			ImGui::TreePop();
		}
		if (need_resize7)
		{
			need_resize7 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width7, io.FontGlobalScale * 200.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width7) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width7, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 200.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 200.0f));
			}
		}
		if (need_sort7)
		{
			need_sort7 = false;
			if (padding7 * 7.0f + width7 * 7.0f * io.FontGlobalScale > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding7 * (float)(smallest_id + 1) + width7 * (float)(smallest_id)*io.FontGlobalScale, last_smallest + padding7));
				h_array.at(smallest_id) += padding7 + io.FontGlobalScale * 200.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding7 * 7.0f + width7 * 6.0f * io.FontGlobalScale, padding7));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 200.0f + padding7);
			}
		}
	}
	ImGui::End();
}


void Label::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base7 = base_;
	cocos_base7 = cocos_base_;
	pHandle7 = pHandle_;
	width7 = width_;
	padding7 = padding_;
}


void Label::sort() {
	need_sort7 = true;
}


void Label::resize() {
	need_resize7 = true;
}
