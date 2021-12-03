#include <Windows.h>


namespace ModsMenu {
	void init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_);
	void draw(ImGuiIO& io, bool enable_tooltip, RECT window_size, ImGuiStyle& style);
	void sort();
	void resize();
}
