#pragma once
#include <cstddef>
#include <cstdint>

namespace patcher {
	void setUChar(uint32_t addr, unsigned char setChar, bool virtualProtect);
	void setRaw(uint32_t addr, const char* setData, uint8_t size, bool virtualProtect);
	void PutRetn(uint32_t addr, bool virtualProtect);

	void set_uint8(uint32_t addr, uint8_t value, bool virtualProtect);
	void set_uint16(uint32_t addr, uint16_t value, bool virtualProtect);
	void set_uint32(uint32_t addr, uint32_t value, bool virtualProtect);
	void set_uint64(uint32_t addr, uint64_t value, bool virtualProtect);

	void fill(uint32_t addr, uint8_t size, uint32_t value, bool virtualProtect);
}