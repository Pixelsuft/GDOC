#define WIN32_LEAN_AND_MEAN
#define CONSOLE_
#include <windows.h>
#include <imgui.h>
#include <imgui-hook.hpp>


void apply_style(ImGuiStyle& style) {
	style.WindowBorderSize = 0.0f;
	style.WindowRounding = 0.0f;
	style.ScrollbarRounding = 12.0f;
	style.GrabRounding = 6.0f;
	style.TabRounding = 6.0f;
	style.WindowTitleAlign.x = 0.5f;

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 162.0f / 255.0f, 232.0f / 255.0f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = style.Colors[ImGuiCol_FrameBgActive];
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 105.0f / 255.0f, 150.0f / 255.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(237.0f / 255.0f, 28.0f / 255.0f, 36.0f / 255.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = style.Colors[ImGuiCol_TitleBg];
	style.Colors[ImGuiCol_TitleBgCollapsed] = style.Colors[ImGuiCol_TitleBg];
	//style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	//style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 242.0f / 255.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = style.Colors[ImGuiCol_FrameBgActive];
}