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
#include "creator_json_def.h"
#include "mem_utils.h"
#include "main_menu.h"
#include "Utils.h"


using namespace std;
using namespace nlohmann;


DWORD base3;
DWORD cocos_base3;
HANDLE pHandle3;
vector<bool> turned_on3;
json data3;
float width3 = 0.0f;
bool need_sort3 = false;
bool need_resize3 = false;
float padding3 = 5.0f;


void CreatorJson::ExportConfig() {
	Utils::export_bool(turned_on3, "gdoc_cfg\\creator.conf");
}


void CreatorJson::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Creator (Default)"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		for (int i = 0; i < data3.size(); i++) {
			json current_obj = data3.at(i);
			bool current = turned_on3.at(i);
			if (ImGui::Checkbox(string(current_obj["name"]).c_str(), &current)) {
				turned_on3.at(i) = current;
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
					DWORD base_new = base3;
					if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
						base_new = cocos_base3;
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
		if (need_resize3)
		{
			need_resize3 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width3, io.FontGlobalScale * 400.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width3) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width3, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 400.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 400.0f));
			}
		}
		if (need_sort3)
		{
			need_sort3 = false;
			if (padding3 * 3.0f + width3 * 3.0f * io.FontGlobalScale > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding3 * (float)(smallest_id + 1) + width3 * (float)(smallest_id)*io.FontGlobalScale, last_smallest + padding3));
				h_array.at(smallest_id) += padding3 + io.FontGlobalScale * 400.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding3 * 3.0f + width3 * 2.0f * io.FontGlobalScale, padding3));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 400.0f + padding3);
			}
		}
	}
	ImGui::End();
}


void CreatorJson::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base3 = base_;
	cocos_base3 = cocos_base_;
	pHandle3 = pHandle_;
	width3 = width_;
	padding3 = padding_;
	ifstream file("GDOC\\creator.json");
	if (!file.is_open())
		return;
	string file_content;
	getline(file, file_content, '\0');
	file.close();
	data3 = json::parse(file_content)["hacks"];
	ifstream myfile;
	myfile.open("gdoc_cfg\\creator.conf");
	for (int i = 0; i < data3.size(); i++) {
		bool to_add = false;
		if (myfile.is_open()) {
			string line_;
			getline(myfile, line_);
			if (line_ == "1") {
				to_add = true;
				json opcodes = data3.at(i)["opcodes"];
				for (int j = 0; j < opcodes.size(); j++) {
					json opcode = opcodes.at(j);
					uintptr_t address;
					sscanf_s(string(opcode["addr"]).data(), "%x", &address);
					string str_bytes(opcode["on"]);
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
					DWORD base_new = base3;
					if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
						base_new = cocos_base3;
					}
					uint32_t addr = base_new + address;
					uint32_t old = MemUtils::Protect(addr, bytes_to_write.size(), PAGE_EXECUTE_READWRITE);
					MemUtils::write_bytes(addr, bytes_to_write);
					MemUtils::Protect(addr, bytes_to_write.size(), old);
				}
			}
		}
		turned_on3.push_back(to_add);
	}
	if (myfile.is_open()) {
		myfile.close();
	}
}


void CreatorJson::sort() {
	need_sort3 = true;
}


void CreatorJson::resize() {
	need_resize3 = true;
}
