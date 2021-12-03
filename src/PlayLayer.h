namespace PlayLayer {
    float& get_float_var(int idx);
    bool& get_bool_var(int idx);
    void set_attempt_format(char* format);
    void update_speedhack(bool a1, bool a2, float a3, float a4);
    void update_rainbow(int idx, int type, float speed, float speed2);
    int& get_int_var(int idx);

    inline bool(__thiscall* init)(gd::PlayLayer* self, void* level);
    bool __fastcall initHook(gd::PlayLayer* self, void*, void* level);

    inline void(__thiscall* update)(gd::PlayLayer* self, float dt);
    void __fastcall updateHook(gd::PlayLayer* self, void*, float dt);

    inline void(__thiscall* resetLevel)(gd::PlayLayer* self);
    void __fastcall resetLevelHook(gd::PlayLayer* self, void*);

    inline void(__thiscall* onQuit)(gd::PlayLayer* self);
    void __fastcall onQuitHook(gd::PlayLayer* self, void*);

    inline bool(__thiscall* pushButton)(gd::PlayerObject* self, void* PlayerButton);
    inline bool(__thiscall* releaseButton)(gd::PlayerObject* self, void* PlayerButton);

    bool __fastcall pushButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton);
    bool __fastcall releaseButtonHook(gd::PlayerObject* self, int edx, void* PlayerButton);

	void mem_init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_);
}