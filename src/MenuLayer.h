namespace MenuLayer {
	inline int(__thiscall* init)(gd::MenuLayer* self);
	int __fastcall initHook(gd::MenuLayer* self);
	bool& get_mg_btn();

	void mem_init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_);
}