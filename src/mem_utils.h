#include <Windows.h>

namespace MemUtils {
	DWORD get_offset(std::vector<DWORD> pointsOffsets);
	bool write_bytes(uintptr_t const address, std::vector<uint8_t> const& bytes);
	void init(DWORD base_, DWORD cocos_base_, HANDLE pHandle_, float width_, float padding_);
	uint32_t Protect(uint32_t vaddress, size_t size, uint32_t new_protect);
}