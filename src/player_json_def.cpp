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
#include "json.hpp"
#include "player_json_def.h"
#include "mem_utils.h"
#include "PlayLayer.h"
#include "main_menu.h"


using namespace std;
using namespace nlohmann;


DWORD base4;
DWORD cocos_base4;
HANDLE pHandle4;
vector<bool> turned_on4;
json data4;
float width4 = 0.0f;
bool need_sort4 = false;
bool need_resize4 = false;
float padding4 = 5.0f;
bool default_mini_icon = false;


void PlayerJson::update_var_player() {
	default_mini_icon = (bool)gd::GameManager::sharedState()->getIntGameVariable("0060");
}


void PlayerJson::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Player (Default)"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		for (int i = 0; i < data4.size(); i++) {
			json current_obj = data4.at(i);
			bool current = turned_on4.at(i);
			if (ImGui::Checkbox(string(current_obj["name"]).c_str(), &current)) {
				turned_on4.at(i) = current;
				json opcodes = current_obj["opcodes"];
				for (int j = 0; j < opcodes.size(); j++) {
					json opcode = opcodes.at(j);
					uintptr_t address;
					sscanf_s(string(opcode["addr"]).data(), "%x", &address);
					string str_bytes(opcode[current ? "on" : "off"]);
					str_bytes.push_back(' ');
					vector<uint8_t> bytes_to_write;
					string current_byte("");
					for (int h = 0; h < str_bytes.size(); h++) {
						if (str_bytes.at(h) == ' ') {
							int cur_bt;
							sscanf_s(string("0x" + current_byte).data(), "%x", &cur_bt);
							bytes_to_write.push_back(cur_bt);
							current_byte.clear();
							continue;
						}
						current_byte.push_back(str_bytes.at(h));
					}
					DWORD base_new = base4;
					if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
						base_new = cocos_base4;
					}
					uint32_t addr = base_new + address;
					uint32_t old = MemUtils::Protect(addr, bytes_to_write.size(), PAGE_EXECUTE_READWRITE);
					MemUtils::write_bytes(addr, bytes_to_write);
					MemUtils::Protect(addr, bytes_to_write.size(), old);
				}
			}
			if (enable_tooltip && ImGui::IsItemHovered()) {
				ImGui::SetTooltip(string(current_obj["desc"]).c_str());
			}
		}

		ImGui::Checkbox("Auto Practice Mode", &PlayLayer::get_bool_var(9));
		ImGui::Checkbox("Practice No Death Win", &PlayLayer::get_bool_var(12));
		ImGui::Checkbox("Practice Mode (No Reset)", &PlayLayer::get_bool_var(8));
		if (ImGui::Checkbox("Default Mini Icon", &default_mini_icon)) {
			gd::GameManager::sharedState()->setIntGameVariable("0060", (int)default_mini_icon);
		}
		ImGui::Text("Respawn Time");
		ImGui::DragFloat("T", &PlayLayer::get_float_var(13), 0.5f, 0.01f, 9999.0f, "%f", 0.0f);
		if (need_resize4)
		{
			need_resize4 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width4, io.FontGlobalScale * 650.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width4) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width4, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 650.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 650.0f));
			}
		}
		if (need_sort4)
		{
			need_sort4 = false;
			if (padding4 * 4.0f + width4 * 4.0f * io.FontGlobalScale > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding4 * (float)(smallest_id + 1) + width4 * (float)(smallest_id)*io.FontGlobalScale, last_smallest + padding4));
				h_array.at(smallest_id) += padding4 + io.FontGlobalScale * 650.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding4 * 4.0f + width4 * 3.0f * io.FontGlobalScale, padding4));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 650.0f + padding4);
			}
		}
	}
	ImGui::End();
}


void PlayerJson::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base4 = base_;
	cocos_base4 = cocos_base_;
	pHandle4 = pHandle_;
	width4 = width_;
	padding4 = padding_;
	ifstream file("GDOC\\player.json");
	if (!file.is_open())
		return;
	string file_content;
	getline(file, file_content, '\0');
	file.close();
	data4 = json::parse(file_content)["hacks"];
	for (int i = 0; i < data4.size(); i++) {
		turned_on4.push_back(false);
	}
}


void PlayerJson::sort() {
	need_sort4 = true;
}


void PlayerJson::resize() {
	need_resize4 = true;
}
