#pragma once

#include <cstdint>

namespace mem {

	/****************************************************************
			Memory Class
	****************************************************************/

	class Memory {

	public:
		Memory(uint32_t nsize);
		~Memory();

		uint8_t read(uint32_t index);
		int8_t readSigned(uint32_t index);

		bool write(uint32_t index, uint8_t value);

	private:
		uint32_t size;
		uint8_t* data;

		bool valid = false;

	};
}