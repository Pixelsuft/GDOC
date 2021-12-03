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
#include "bypass_json_def.h"
#include "mem_utils.h"
#include "main_menu.h"


using namespace std;
using namespace nlohmann;


DWORD base1;
DWORD cocos_base1;
HANDLE pHandle1;
vector<bool> turned_on1;
json data1;
float width1 = 0.0f;
bool need_sort1 = false;
bool need_resize1 = false;
float padding1 = 5.0f;


void BypassJson::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Bypass (Default)"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		for (int i = 0; i < data1.size(); i++) {
			json current_obj = data1.at(i);
			bool current = turned_on1.at(i);
			if (ImGui::Checkbox(string(current_obj["name"]).c_str(), &current)) {
				turned_on1.at(i) = current;
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
					uint32_t addr = base1 + address;
					uint32_t old = MemUtils::Protect(addr, bytes_to_write.size(), PAGE_EXECUTE_READWRITE);
					MemUtils::write_bytes(addr, bytes_to_write);
					MemUtils::Protect(addr, bytes_to_write.size(), old);
				}
			}
			if (enable_tooltip && ImGui::IsItemHovered()) {
				ImGui::SetTooltip(string(current_obj["desc"]).c_str());
			}
		}
		if (need_resize1)
		{
			need_resize1 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width1, io.FontGlobalScale * 400.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width1) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width1, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 400.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 400.0f));
			}
		}
		if (need_sort1)
		{
			need_sort1 = false;
			ImGui::SetWindowPos(ImVec2(padding1, padding1));
			MainMenu::get_h_array().clear();
			MainMenu::get_h_array().push_back(io.FontGlobalScale * 400.0f + padding1);
		}
	}
	ImGui::End();
}


void BypassJson::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base1 = base_;
	cocos_base1 = cocos_base_;
	pHandle1 = pHandle_;
	width1 = width_;
	padding1 = padding_;
	ifstream file("GDOC\\bypass.json");
	if (!file.is_open())
		return;
	string file_content;
	getline(file, file_content, '\0');
	file.close();
	data1 = json::parse(file_content)["hacks"];
	for (int i = 0; i < data1.size(); i++) {
		turned_on1.push_back(false);
	}
}


void BypassJson::sort() {
	need_sort1 = true;
}


void BypassJson::resize() {
	need_resize1 = true;
}
