namespace MainMenu {
	bool& get_b(int idx);
	int& get_i(int idx);
	void set_tt(bool new_tt, float tt_time);
	std::vector<float> &get_h_array();
	void init();
	void draw();
	void SortWindows();
	void ResizeWindows();
	void SetHwnd(HWND hwnd__);
	void ProcessKey(WPARAM key__, bool is_down);
	HWND GetHwnd();
	void set_font_id(int new_id);
	void ExportConfig();
}