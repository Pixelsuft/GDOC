#include <iostream>
#include <cstdlib>
#include <vector>
#include <Windows.h>
#include "mem_utils.h"


using namespace std;
DWORD base0;
DWORD cocos_base0;
HANDLE pHandle0;


DWORD MemUtils::get_offset(std::vector<DWORD> pointsOffsets) {
    DWORD pointsAddress = base0;
    for (int i = 0; i < pointsOffsets.size() - 1; i++)
    {
        ReadProcessMemory(pHandle0, (LPVOID)(pointsAddress + pointsOffsets.at(i)), &pointsAddress, sizeof(pointsAddress), NULL);
    }
    pointsAddress += pointsOffsets.at(pointsOffsets.size() - 1);
    return pointsAddress;
}


bool MemUtils::write_bytes(uintptr_t const address, std::vector<uint8_t> const& bytes) {
	return WriteProcessMemory(pHandle0, reinterpret_cast<LPVOID>(address), bytes.data(), bytes.size(), nullptr);
}


void MemUtils::init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_) {
	base0 = base_;
	cocos_base0 = cocos_base_;
	pHandle0 = pHandle_;
}

uint32_t MemUtils::Protect(uint32_t vaddress, size_t size, uint32_t new_protect)
{
	DWORD old;
	return VirtualProtectEx(pHandle0, reinterpret_cast<void*>(vaddress), size, new_protect, &old) ? static_cast<uint32_t>(old) : 0;
}
