#define WIN32_LEAN_AND_MEAN
#define CONSOLE_
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
#include "main_menu.h"
#include "MenuLayer.h"
#include "PlayLayer.h"
#include "SpeedhackAudio.h"


using namespace cocos2d;
using namespace std;
using namespace std::chrono;


DWORD WINAPI my_thread(void* hModule) {
#ifdef CONSOLE_
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "w", stdin);
#endif
    MainMenu::init();

    MH_Initialize();

    ImGuiHook::setupHooks([](void* target, void* hook, void** trampoline) {
        MH_CreateHook(target, hook, trampoline);
    });

    MenuLayer::mem_init((DWORD)GetModuleHandle(0), (DWORD)GetModuleHandle("libcocos2d.dll"), GetCurrentProcess());
    PlayLayer::mem_init((DWORD)GetModuleHandle(0), (DWORD)GetModuleHandle("libcocos2d.dll"), GetCurrentProcess());
    SpeedhackAudio::init();

    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        CreateThread(0, 0, my_thread, module, 0, 0);
    }
    return TRUE;
}
