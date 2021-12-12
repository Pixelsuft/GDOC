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
#include "gdoc_menu.h"


using namespace std;


DWORD base9;
DWORD cocos_base9;
HANDLE pHandle9;
vector<bool> turned_on9;
float width9 = 0.0f;
bool need_sort9 = false;
bool need_resize9 = false;
bool disable_tt = false;
float tt_time = 0.5f;
float padding9 = 5.0f;
int font_id9 = 0;


void GdocMenu::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size, ImGuiStyle& style) {
	if (ImGui::Begin("GDOC"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		ImGui::Text("Mod Menu: "); ImGui::SameLine();
		if (ImGui::Button("Sort")) {
			MainMenu::SortWindows();
		} ImGui::SameLine();
		if (ImGui::Button("Resize")) {
			MainMenu::ResizeWindows();
		}
		if (ImGui::Checkbox("Disable ToolTip", &disable_tt)) {
			MainMenu::set_tt(!disable_tt, tt_time);			
		}
		if (ImGui::DragFloat("Time", &tt_time, 0.05f, 0.00f, 5.0f, "%f", 0.0f)) {
			MainMenu::set_tt(!disable_tt, tt_time);
		}
		if (ImGui::DragFloat("Scale", &io.FontGlobalScale, 0.005f, 0.3f, 3.0f, "%f", ImGuiSliderFlags_AlwaysClamp)) {
			MainMenu::ResizeWindows();
			MainMenu::SortWindows();
		}
		ImGui::DragFloat("Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%f");
		if (ImGui::TreeNode("Font")) {
			if (ImGui::RadioButton("Font 0", &font_id9, 0)) {
				MainMenu::set_font_id(font_id9);
			}
			if (ImGui::RadioButton("Font 1", &font_id9, 1)) {
				MainMenu::set_font_id(font_id9);
			}
			if (ImGui::RadioButton("Font 2", &font_id9, 2)) {
				MainMenu::set_font_id(font_id9);
			}
			ImGui::TreePop();
		}
		ImGui::Text("Hover On Text To Change");
		ImGui::Text(("Menu Bind: [" + to_string(MainMenu::get_i(0)) + "]").c_str());
		MainMenu::get_b(0) = ImGui::IsItemHovered();
		ImGui::Text(("ScreenShot Bind: [" + to_string(MainMenu::get_i(1)) + "]").c_str());
		MainMenu::get_b(1) = ImGui::IsItemHovered();
		if (ImGui::Button("Save Config")) {
			MainMenu::ExportConfig();
		}
		
		if (need_resize9)
		{
			need_resize9 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width9, io.FontGlobalScale * 400.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width9) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width9, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 400.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 400.0f));
			}
		}
		if (need_sort9)
		{
			need_sort9 = false;
			if (padding9 * 9.0f + width9 * 9.0f * io.FontGlobalScale > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding9 * (float)(smallest_id + 1) + width9 * (float)(smallest_id)*io.FontGlobalScale, last_smallest + padding9));
				h_array.at(smallest_id) += padding9 + io.FontGlobalScale * 400.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding9 * 9.0f + width9 * 8.0f * io.FontGlobalScale, padding9));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 400.0f + padding9);
			}
		}
	}
	ImGui::End();
}


void GdocMenu::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base9 = base_;
	cocos_base9 = cocos_base_;
	pHandle9 = pHandle_;
	width9 = width_;
	padding9 = padding_;
}


void GdocMenu::sort() {
	need_sort9 = true;
}


void GdocMenu::resize() {
	need_resize9 = true;
}
