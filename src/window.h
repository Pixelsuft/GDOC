#include <Windows.h>


namespace Window {
	void init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_);
	void draw(ImGuiIO& io, bool enable_tooltip, RECT window_size);
	void sort();
	void resize();
	void hwnd_init(HWND hwnd__);
	void MakeScreenShot();
}
