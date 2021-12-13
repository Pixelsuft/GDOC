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
#include "global_json_def.h"
#include "mem_utils.h"
#include "main_menu.h"
#include "Utils.h"


using namespace std;
using namespace nlohmann;


DWORD base2;
DWORD cocos_base2;
HANDLE pHandle2;
vector<bool> turned_on2;
json data2;
float width2 = 0.0f;
bool need_sort2 = false;
bool need_resize2 = false;
float padding2 = 5.0f;

bool remove_filters = false;

float l1[4] = { 191.0f / 255.0f, 114.0f / 255.0f, 62.0f / 255.0f, 1.0f };
float l2[4] = { 161.0f / 255.0f, 88.0f / 255.0f, 44.0f / 255.0f, 1.0f };
float l3[4] = { 194.0f / 255.0f, 114.0f / 255.0f, 62.0f / 255.0f, 1.0f };
float mcc1[4] = { 0.0f, 102.0f / 255.0f, 1.0f, 1.0f };


void GlobalJson::ExportConfig() {
	Utils::export_bool(turned_on2, "gdoc_cfg\\global.conf");
}


void GlobalJson::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Global (Default)"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		for (int i = 0; i < data2.size(); i++) {
			json current_obj = data2.at(i);
			bool current = turned_on2.at(i);
			if (ImGui::Checkbox(string(current_obj["name"]).c_str(), &current)) {
				turned_on2.at(i) = current;
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
					DWORD base_new = base2;
					if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
						base_new = cocos_base2;
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

		ImGui::Text("List Colors");
		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			l1[0] = 191.0f / 255.0f;
			l1[1] = 114.0f / 255.0f;
			l1[2] = 62.0f / 255.0f;
			l1[3] = 1.0f;
			l2[0] = 161.0f / 255.0f;
			l2[1] = 88.0f / 255.0f;
			l2[2] = 44.0f / 255.0f;
			l2[3] = 1.0f;
			l3[0] = 194.0f / 255.0f;
			l3[1] = 114.0f / 255.0f;
			l3[2] = 62.0f / 255.0f;
			l3[3] = 1.0f;
		}

		if (ImGui::ColorEdit4("L1", l1, ImGuiColorEditFlags_AlphaBar)) {
			l2[3] = l3[3] = l1[3];
			MemUtils::write_bytes(base2 + 0x15C02C, { (uint8_t)(l1[0] * 255.0f), (uint8_t)(l1[1] * 255.0f), (uint8_t)(l1[2] * 255.0f), (uint8_t)(l1[3] * 255.0f) });
		}

		if (ImGui::ColorEdit4("L2", l2, ImGuiColorEditFlags_AlphaBar)) {
			l2[3] = l3[3] = l2[3];
			MemUtils::write_bytes(base2 + 0x5C70A, { (uint8_t)(l2[3] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x5C6D9, { (uint8_t)(l2[0] * 255.0f), (uint8_t)(l2[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x5C6DC, { (uint8_t)(l2[2] * 255.0f) });
		}

		if (ImGui::ColorEdit4("L3", l3, ImGuiColorEditFlags_AlphaBar)) {
			l2[3] = l1[3] = l3[3];
			MemUtils::write_bytes(base2 + 0x5C70A, { (uint8_t)(l3[3] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x5C6CF, { (uint8_t)(l3[0] * 255.0f), (uint8_t)(l3[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x5C6D2, { (uint8_t)(l3[2] * 255.0f) });
		}

		ImGui::Text("Online Menu Color");
		ImGui::SameLine();

		bool btn_reset = ImGui::Button("Reset");

		if (btn_reset) {
			mcc1[0] = 0.0f;
			mcc1[1] = 102.0f / 255.0f;
			mcc1[2] = 1.0f;
			mcc1[3] = 1.0f;
		}

		if (ImGui::ColorEdit4("M1", mcc1, 0) || btn_reset) {
			mcc1[3] = 1.0f;
			MemUtils::write_bytes(base2 + 0x15A174, { (uint8_t)(mcc1[0] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x15A175, { (uint8_t)(mcc1[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x15A16F, { (uint8_t)(mcc1[2] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x15891D, { (uint8_t)(mcc1[0] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x15891E, { (uint8_t)(mcc1[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x158917, { (uint8_t)(mcc1[2] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x6F7FB, { (uint8_t)(mcc1[0] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x6F7FC, { (uint8_t)(mcc1[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x6F7F6, { (uint8_t)(mcc1[2] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x1979AD, { (uint8_t)(mcc1[0] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x1979AE, { (uint8_t)(mcc1[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x1979A7, { (uint8_t)(mcc1[2] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x17DBC1, { (uint8_t)(mcc1[0] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x17DBC2, { (uint8_t)(mcc1[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x17DBBB, { (uint8_t)(mcc1[2] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x176032, { (uint8_t)(mcc1[0] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x176033, { (uint8_t)(mcc1[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x176036, { (uint8_t)(mcc1[2] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x4DF7E, { (uint8_t)(mcc1[0] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x4DF7F, { (uint8_t)(mcc1[1] * 255.0f) });
			MemUtils::write_bytes(base2 + 0x4DF78, { (uint8_t)(mcc1[2] * 255.0f) });
		}

		if (ImGui::Checkbox("Remove Filters", &remove_filters)) {
			vector<uint8_t> need_bytes = { (uint8_t)(remove_filters ? 0x90 : 0x80), (uint8_t)(remove_filters ? 0xB1 : 0xC9) };
			MemUtils::write_bytes(base2 + 0x15A16D, need_bytes);
			MemUtils::write_bytes(base2 + 0x158915, need_bytes);
			MemUtils::write_bytes(base2 + 0x6F7F4, need_bytes);
			MemUtils::write_bytes(base2 + 0x1979A5, need_bytes);
			MemUtils::write_bytes(base2 + 0x17DBB9, need_bytes);
			MemUtils::write_bytes(base2 + 0x176034, need_bytes);
			MemUtils::write_bytes(base2 + 0x4DF76, need_bytes);
		}

		if (ImGui::Button("Open Settings")) {
			gd::OptionsLayer::addToCurrentScene();
		}

		if (need_resize2)
		{
			need_resize2 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width2, io.FontGlobalScale * 600.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width2) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width2, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 600.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 600.0f));
			}
		}
		if (need_sort2)
		{
			need_sort2 = false;
			if (padding2 * 2.0f + width2 * io.FontGlobalScale * 2.0f > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding2 * (float)(smallest_id + 1) + width2 * (float)(smallest_id) * io.FontGlobalScale, last_smallest + padding2));
				h_array.at(smallest_id) += padding2 + io.FontGlobalScale * 600.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding2 * 2.0f + width2 * io.FontGlobalScale, padding2));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 600.0f + padding2);
			}
		}
	}
	ImGui::End();
}


void GlobalJson::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base2 = base_;
	cocos_base2 = cocos_base_;
	pHandle2 = pHandle_;
	width2 = width_;
	padding2 = padding_;
	ifstream file("GDOC\\global.json");
	if (!file.is_open())
		return;
	string file_content;
	getline(file, file_content, '\0');
	file.close();
	data2 = json::parse(file_content)["hacks"];
	ifstream myfile;
	myfile.open("gdoc_cfg\\global.conf");
	for (int i = 0; i < data2.size(); i++) {
		bool to_add = false;
		if (myfile.is_open()) {
			string line_;
			getline(myfile, line_);
			if (line_ == "1") {
				to_add = true;
				json opcodes = data2.at(i)["opcodes"];
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
					DWORD base_new = base2;
					if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
						base_new = cocos_base2;
					}
					uint32_t addr = base_new + address;
					uint32_t old = MemUtils::Protect(addr, bytes_to_write.size(), PAGE_EXECUTE_READWRITE);
					MemUtils::write_bytes(addr, bytes_to_write);
					MemUtils::Protect(addr, bytes_to_write.size(), old);
				}
			}
		}
		turned_on2.push_back(to_add);
	}
	if (myfile.is_open()) {
		myfile.close();
	}
}


void GlobalJson::sort() {
	need_sort2 = true;
}


void GlobalJson::resize() {
	need_resize2 = true;
}
