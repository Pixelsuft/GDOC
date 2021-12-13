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
#include "window.h"
#include "PlayLayer.h"
#include "MenuLayer.h"
#include "SpeedhackAudio.h"


using namespace std;


DWORD base6;
DWORD cocos_base6;
HANDLE pHandle6;
HWND hwnd6;
vector<bool> turned_on6;
float width6 = 0.0f;
bool need_sort6 = false;
bool need_resize6 = false;
bool fullscreen_windowed = false;
float default_fps = 60.0f;
float fps = default_fps;
bool enable_speedhack = false;
bool enable_speedhack_audio = false;
float speedhack_speed = 1.0f;
float audio_speedhack_speed = 1.0f;
bool no_sync_audio_speedhack = false;
float padding6 = 5.0f;
RECT gd_rect_;
LONG gd_rect_long_ = 0;
char window_title[256] = "Geometry Dash";


void Window::MakeScreenShot() {
	// https://stackoverflow.com/questions/7292757/how-to-get-screenshot-of-a-window-as-bitmap-object-in-c
	HWND hwnd = MainMenu::GetHwnd();
	RECT window_size;
	GetClientRect(hwnd, &window_size);
	HDC hdcScreen = GetDC(hwnd);
	HDC hdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmp = CreateCompatibleBitmap(
		hdcScreen,
		window_size.right,
		window_size.bottom
	);
	SelectObject(hdc, hbmp);
	PrintWindow(hwnd, hdc, PW_CLIENTONLY);
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hbmp);
	CloseClipboard();
	DeleteDC(hdc);
	DeleteObject(hbmp);
	ReleaseDC(NULL, hdcScreen);
}


void Window::draw(ImGuiIO& io, bool enable_tooltip, RECT window_size) {
	if (ImGui::Begin("Window/Frame/Speed"), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize) {
		ImGui::Text("SpeedHacks");
		if (ImGui::Checkbox("Enable SpeedHack", &enable_speedhack)) {
			PlayLayer::update_speedhack(enable_speedhack, enable_speedhack_audio, speedhack_speed, audio_speedhack_speed);
		}
		if (ImGui::Checkbox("Enable Audio SpeedHack", &enable_speedhack_audio)) {
			PlayLayer::update_speedhack(enable_speedhack, enable_speedhack_audio, speedhack_speed, audio_speedhack_speed);
		}
		if (ImGui::Checkbox("Do Not Sync Audio Speed", &no_sync_audio_speedhack)) {
			if (!no_sync_audio_speedhack) {
				audio_speedhack_speed = speedhack_speed;
			}
			PlayLayer::update_speedhack(enable_speedhack, enable_speedhack_audio, speedhack_speed, audio_speedhack_speed);
		}
		if (ImGui::DragFloat("Speed", &speedhack_speed, 0.5f, 0.0f, 999.0f, "%f", 0.0f)) {
			if (!no_sync_audio_speedhack)
				audio_speedhack_speed = speedhack_speed;
			PlayLayer::update_speedhack(enable_speedhack, enable_speedhack_audio, speedhack_speed, audio_speedhack_speed);
		}
		if (ImGui::DragFloat("Audio", &audio_speedhack_speed, 0.5f, 0.0f, 999.0f, "%f", 0.0f)) {
			PlayLayer::update_speedhack(enable_speedhack, enable_speedhack_audio, speedhack_speed, audio_speedhack_speed);
		}

		ImGui::DragFloat("Shitty", &PlayLayer::get_float_var(0), 0.5f, 0.3f, 999.0f, "%f", 0.0f);
		if (ImGui::Button("Force Apply (Beta)")) {
			cocos2d::CCDirector::sharedDirector()->getScheduler()->setTimeScale(enable_speedhack ? speedhack_speed : 1.0f);
			SpeedhackAudio::set(enable_speedhack_audio ? audio_speedhack_speed : 1.0f);
		}

		ImGui::Text("In-Game FPS");
		ImGui::Checkbox("New Delta", &PlayLayer::get_bool_var(1));
		if (ImGui::IsItemHovered() && enable_tooltip) {
			ImGui::SetTooltip("New FPS System. Maybe can fix something?");
		}
		ImGui::Checkbox("Enable FPS Bypass", &PlayLayer::get_bool_var(6));
		ImGui::DragInt("Fake +", &PlayLayer::get_int_var(0), 1.0f, 1, 999, "%i", 0);
		if (ImGui::DragFloat("Bypass", &fps, 5.0f, 0.01f, 99999.0f, "%f", 0.0f)) {
			PlayLayer::get_float_var(2) = 1.0f / fps;
		}
		if (ImGui::Button("Reset")) {
			fps = default_fps;
			PlayLayer::get_float_var(2) = 1.0f / fps;
		}

		ImGui::Text("Window");
		if (ImGui::InputText("Title", window_title, sizeof(window_title))) {
			SetWindowTextA(hwnd6, window_title);
		}
		if (ImGui::Button("ScreenShot to ClipBoard")) {
			MakeScreenShot();
		}

		ImGui::Text("Other");
		ImGui::Checkbox("Hide More Games Button", &MenuLayer::get_mg_btn());
		if (ImGui::Checkbox("Fullscreen Windowed Mode", &fullscreen_windowed)) {
			if (fullscreen_windowed) {
				uint8_t check_bt;
				ReadProcessMemory(
					pHandle6,
					reinterpret_cast<void*>(cocos_base6 + 0x11388B),
					&check_bt,
					sizeof(check_bt),
					NULL
				);
				if (check_bt == 0x90 || (MessageBox(
					MainMenu::GetHwnd(),
					"You have not checked 'Free Window Resize Option'.\nContinue?",
					"Warning!",
					MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2
				) == IDYES)) {
					HWND gd_hwnd = MainMenu::GetHwnd();
					HWND desktop = GetDesktopWindow();
					RECT screen_size;
					GetClientRect(desktop, &screen_size);
					GetWindowRect(gd_hwnd, &gd_rect_);
					LONG gd_rect_long_1 = gd_rect_long_ == 0 ? GetWindowLong(gd_hwnd, GWL_STYLE) : gd_rect_long_;
					gd_rect_long_ = gd_rect_long_1;
					gd_rect_long_1 &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
					SetWindowLong(gd_hwnd, GWL_STYLE, gd_rect_long_1);
					MoveWindow(gd_hwnd, 0, 0, screen_size.right, screen_size.bottom, false);
				}
				else {
					fullscreen_windowed = false;
				}
			}
			else {
				HWND gd_hwnd = MainMenu::GetHwnd();
				SetWindowLong(gd_hwnd, GWL_STYLE, gd_rect_long_);
				MoveWindow(gd_hwnd, gd_rect_.left, gd_rect_.top, gd_rect_.right - gd_rect_.left, gd_rect_.bottom - gd_rect_.top, false);
			}
		}
		if (ImGui::IsItemHovered() && enable_tooltip) {
			ImGui::SetTooltip("Make sure that you running in windowed mode with checked 'Free Window Resize' option");
		}

		if (need_resize6)
		{
			need_resize6 = false;
			ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width6, io.FontGlobalScale * 480.0f));
		}
		else
		{
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x > io.FontGlobalScale * width6) {
				ImGui::SetWindowSize(ImVec2(io.FontGlobalScale * width6, window_size.y));
			}
			if (window_size.y > io.FontGlobalScale * 480.0f) {
				ImGui::SetWindowSize(ImVec2(window_size.x, io.FontGlobalScale * 480.0f));
			}
		}
		if (need_sort6)
		{
			need_sort6 = false;
			if (padding6 * 6.0f + width6 * io.FontGlobalScale * 6.0f > window_size.right) {
				vector<float>& h_array = MainMenu::get_h_array();
				float last_smallest = h_array.at(0);
				int smallest_id = 0;
				for (int i = 0; i < h_array.size(); i++) {
					if (h_array.at(i) > last_smallest)
						continue;
					smallest_id = i;
					last_smallest = h_array.at(i);
				}
				ImGui::SetWindowPos(ImVec2(padding6 * (float)(smallest_id + 1) + width6 * io.FontGlobalScale * (float)smallest_id, last_smallest + padding6));
				h_array.at(smallest_id) += padding6 + io.FontGlobalScale * 480.0f;
			}
			else {
				ImGui::SetWindowPos(ImVec2(padding6 * 6.0f + width6 * 5.0f * io.FontGlobalScale, padding6));
				MainMenu::get_h_array().push_back(io.FontGlobalScale * 480.0f + padding6);
			}
		}
	}
	ImGui::End();
}

void Window::hwnd_init(HWND hwnd__) {
	hwnd6 = hwnd__;
	GetWindowTextA(hwnd6, window_title, sizeof(window_title));
}

void Window::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base6 = base_;
	cocos_base6 = cocos_base_;
	pHandle6 = pHandle_;
	width6 = width_;
	padding6 = padding_;
	DEVMODE dm;
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm)) {
		default_fps = (float)dm.dmDisplayFrequency;
		fps = default_fps;
		PlayLayer::get_float_var(2) = 1.0f / fps;
		PlayLayer::get_float_var(3) = 1.0f / fps;
	}
}


void Window::sort() {
	need_sort6 = true;
}


void Window::resize() {
	need_resize6 = true;
}
