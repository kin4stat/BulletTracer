#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "patcher.h"

namespace patcher {
	void setUChar(uint32_t addr, unsigned char setChar, bool virtualProtect)
	{
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, 1, PAGE_READWRITE, &oldProt);
		*(uint8_t*)(addr) = setChar;
		if (virtualProtect) VirtualProtect((void*)addr, 1, oldProt, &oldProt);
	}
	void setRaw(uint32_t addr, const char* setData, uint8_t size, bool virtualProtect)
	{
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, size, PAGE_READWRITE, &oldProt);
		memcpy((void*)(addr), setData, size);
		if (virtualProtect) VirtualProtect((void*)addr, size, oldProt, &oldProt);
	}
	void PutRetn(uint32_t addr, bool virtualProtect) {
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(BYTE), PAGE_READWRITE, &oldProt);
		*(BYTE*)addr = 0xC3;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(BYTE), oldProt, &oldProt);
	}

	void set_uint8(uint32_t addr, uint8_t value, bool virtualProtect)
	{
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint8_t), PAGE_READWRITE, &oldProt);
		*(uint8_t*)addr = value;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint8_t), oldProt, &oldProt);
	}
	void set_uint16(uint32_t addr, uint16_t value, bool virtualProtect)
	{
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint16_t), PAGE_READWRITE, &oldProt);
		*(uint16_t*)addr = value;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint16_t), oldProt, &oldProt);
	}
	void set_uint32(uint32_t addr, uint32_t value, bool virtualProtect)
	{
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint32_t), PAGE_READWRITE, &oldProt);
		*(uint32_t*)addr = value;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint32_t), oldProt, &oldProt);
	}
	void set_uint64(uint32_t addr, uint64_t value, bool virtualProtect)
	{
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint64_t), PAGE_READWRITE, &oldProt);
		*(uint64_t*)addr = value;
		if (virtualProtect) VirtualProtect((void*)addr, sizeof(uint64_t), oldProt, &oldProt);
	}

	void fill(uint32_t addr, uint8_t size, uint32_t value, bool virtualProtect) 
	{
		DWORD oldProt;
		if (virtualProtect) VirtualProtect((void*)addr, size, PAGE_READWRITE, &oldProt);
		memset((void*)addr, value, size);
		if (virtualProtect) VirtualProtect((void*)addr, size, oldProt, &oldProt);
	}
}