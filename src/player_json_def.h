#include <Windows.h>


namespace PlayerJson {
	void update_var_player();
	void init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_);
	void draw(ImGuiIO& io, bool enable_tooltip, RECT window_size);
	void sort();
	void resize();
}
