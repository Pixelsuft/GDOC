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
#include <algorithm>
#include <regex>
#include "Utils.h"


using namespace std;
using namespace cocos2d;


namespace Utils {
	void export_bool(vector<bool> to_export, string filename) {
		ofstream myfile;
		myfile.open(filename);
		for (int i = 0; i < to_export.size(); i++) {
			myfile << (to_export.at(i) ? "1" : "0") << "\n";
		}
		myfile.close();
	}
}