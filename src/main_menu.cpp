#include <windows.h>
#include <cocos2d.h>
#include <imgui.h>
#include <MinHook.h>
#include <queue>
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <imgui-hook.hpp>
#include <gd.h>
#include "style.hpp"
#include "main_menu.h"
#include "bypass_json_def.h"
#include "global_json_def.h"
#include "creator_json_def.h"
#include "player_json_def.h"
#include "mem_utils.h"
#include "misc.h"
#include "window.h"
#include "label.h"
#include "rainbow.h"
#include "gdoc_menu.h"
#include "mods_menu.h"
#include "PlayLayer.h"


using namespace std;
using namespace std::chrono;
using namespace cocos2d;


float default_width = 200.0f;
float padding = 5.0f;
bool g_showWindow = false;
HWND hwnd = 0;
bool checkbox_check = false;
POINT last_cursor_pos;
uint64_t last_anim_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
DWORD base;
DWORD cocos_base;
HANDLE pHandle;
float used_w = 0.0f;
float used_h = 0.0f;
std::vector<float> h_array1;
bool enable_tt = true;
uint64_t time_tt = 500;
int menu_key = VK_TAB;
bool input_menu_key = false;
int screenshot_key = 'F';
bool input_screenshot_key = false;
bool inited = false;
int font_id = 0;
vector<ImFont*> fonts_list;


void MainMenu::set_tt(bool new_tt, float tt_time) {
    enable_tt = new_tt;
    time_tt = (uint64_t)(tt_time * 1000.0f);
}


void MainMenu::set_font_id(int new_id) {
    font_id = new_id;
}


bool& MainMenu::get_b(int idx) {
    std::filesystem::current_path();
    if (idx == 0)
        return input_menu_key;
    if (idx == 1)
        return input_screenshot_key;
    if (idx == 2)
    {
        inited = true;
        MainMenu::init();
        return inited;
    }
    return input_menu_key;
}


int& MainMenu::get_i(int idx) {
    if (idx == 0)
        return menu_key;
    if (idx == 1)
        return screenshot_key;
    return menu_key;
}


vector<float>& MainMenu::get_h_array() {
    return h_array1;
}


void MainMenu::draw() {
    if (!inited)
        return;
    if (g_showWindow) {
        uint64_t now_anim_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();
        apply_style(style);

        POINT cursor_pos;
        RECT window_size;
        GetCursorPos(&cursor_pos);
        GetClientRect(hwnd, &window_size);

        bool enable_tooltip = false;

        if (cursor_pos.x != last_cursor_pos.x || cursor_pos.y != last_cursor_pos.y) {
            last_cursor_pos = cursor_pos;
            last_anim_time = now_anim_time;
        }
        else if (now_anim_time > last_anim_time + time_tt) {
            enable_tooltip = enable_tt;
        }

        ImGui::PushFont(fonts_list.at(font_id));

        BypassJson::draw(io, enable_tooltip, window_size);
        GlobalJson::draw(io, enable_tooltip, window_size);
        CreatorJson::draw(io, enable_tooltip, window_size);
        PlayerJson::draw(io, enable_tooltip, window_size);
        Misc::draw(io, enable_tooltip, window_size);
        Window::draw(io, enable_tooltip, window_size);
        Label::draw(io, enable_tooltip, window_size);
        Rainbow::draw(io, enable_tooltip, window_size);
        GdocMenu::draw(io, enable_tooltip, window_size, style);
        ModsMenu::draw(io, enable_tooltip, window_size, style);
    }
}


void MainMenu::ProcessKey(WPARAM key__, bool is_down) {
    if (!inited)
        return;
    if (is_down) {
        if (input_menu_key)
        {
            menu_key = key__;
            return;
        }
        else if (input_screenshot_key)
        {
            screenshot_key = key__;
            return;
        }
        else if (key__ == menu_key)
        {
            g_showWindow = !g_showWindow;
            return;
        }
        else if (key__ == screenshot_key)
        {
            Window::MakeScreenShot();
            return;
        }
        else if (key__ == 'A') {
            PlayLayer::get_bool_var(14) = false;
            PlayLayer::get_bool_var(13) = true;
        }
        else if (key__ == 'D') {
            PlayLayer::get_bool_var(13) = false;
            PlayLayer::get_bool_var(14) = true;
        }
        else if (key__ == 'W') {
            PlayLayer::get_bool_var(16) = false;
            PlayLayer::get_bool_var(15) = true;
        }
        else if (key__ == 'S') {
            PlayLayer::get_bool_var(15) = false;
            PlayLayer::get_bool_var(14) = true;
        }
    }
    else {
        if (key__ == 'A') {
            PlayLayer::get_bool_var(13) = false;
        }
        else if (key__ == 'D') {
            PlayLayer::get_bool_var(14) = false;
        }
        else if (key__ == 'W') {
            PlayLayer::get_bool_var(15) = false;
        }
        else if (key__ == 'S') {
            PlayLayer::get_bool_var(16) = false;
        }
    }
}


void MainMenu::SetHwnd(HWND hwnd__) {
    hwnd = hwnd__;
    ImGuiIO& io = ImGui::GetIO();
    fonts_list.push_back(io.Fonts->AddFontDefault());
    fonts_list.push_back(io.Fonts->AddFontFromFileTTF("GDOC\\font1.ttf", 13.0f));
    fonts_list.push_back(io.Fonts->AddFontFromFileTTF("GDOC\\font2.ttf", 13.0f));
    Window::hwnd_init(hwnd__);
}


void MainMenu::ExportConfig() {
    BypassJson::ExportConfig();
}


HWND MainMenu::GetHwnd() {
    return hwnd;
}


void MainMenu::SortWindows() {
    BypassJson::sort();
    GlobalJson::sort();
    CreatorJson::sort();
    PlayerJson::sort();
    Misc::sort();
    Window::sort();
    Label::sort();
    Rainbow::sort();
    GdocMenu::sort();
    ModsMenu::sort();
}


void MainMenu::ResizeWindows() {
    BypassJson::resize();
    GlobalJson::resize();
    CreatorJson::resize();
    PlayerJson::resize();
    Misc::resize();
    Window::resize();
    Label::resize();
    Rainbow::resize();
    GdocMenu::resize();
    ModsMenu::resize();
}


void MainMenu::init() {
    base = (DWORD)GetModuleHandle(0);
    cocos_base = (DWORD)GetModuleHandle("libcocos2d.dll");
    pHandle = GetCurrentProcess();

    // Change imgui.ini path (not working)
    /*TCHAR char_path[MAX_PATH];
    GetModuleFileName(
        GetModuleHandle(0),
        char_path,
        _countof(char_path)
    );
    string exe_path(char_path);
    for (int i = 0; i < exe_path.size(); i++) {
        if (exe_path.at(i) == '/') {
            exe_path.at(i) = '\\';
        }
    }
    string last_element(exe_path.substr(exe_path.rfind("\\") + 1));
    string no_ext = last_element.substr(0, last_element.size() - 4);

    cout << string(std::getenv("LOCALAPPDATA")) + "\\" + no_ext + "\\gdoc_windows.ini" << endl;*/

    if (!filesystem::is_directory("gdoc_cfg")) {
        filesystem::create_directory("gdoc_cfg");
    }

    MemUtils::init(base, cocos_base, pHandle, default_width, padding);
    BypassJson::init(base, cocos_base, pHandle, default_width, padding);
    GlobalJson::init(base, cocos_base, pHandle, default_width, padding);
    CreatorJson::init(base, cocos_base, pHandle, default_width, padding);
    PlayerJson::init(base, cocos_base, pHandle, default_width, padding);
    Misc::init(base, cocos_base, pHandle, default_width, padding);
    Window::init(base, cocos_base, pHandle, default_width, padding);
    Label::init(base, cocos_base, pHandle, default_width, padding);
    Rainbow::init(base, cocos_base, pHandle, default_width, padding);
    GdocMenu::init(base, cocos_base, pHandle, default_width, padding);
    ModsMenu::init(base, cocos_base, pHandle, default_width, padding);

    MainMenu::SortWindows();
    MainMenu::ResizeWindows();
}