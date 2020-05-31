#include "Memory.hpp"

#include <new>
#include <iostream>

using namespace mem;

/****************************************************************
			Memory Class
****************************************************************/

Memory::Memory(uint32_t nsize) {
	size = nsize;
	try {
		data = new uint8_t[size];
	}
	catch (std::bad_alloc& e) {
		std::cout << "Memory::Memory(" << nsize << ") failed: " << e.what() << std::endl;
		return;
	}

	valid = true;
}

Memory::~Memory() {
	delete[] data; // Make sure our mess is cleaned up
}

uint8_t Memory::read(uint32_t index) {
	if (index < 0 || index >= size) {
		std::cout << "Memory::read(" << index << ") out of bounds exception (size = " << size << ")" << std::endl;
		return 0;
	}

	return data[index];
}

int8_t Memory::readSigned(uint32_t index) {
	return (int8_t)read(index);
}

bool Memory::write(uint32_t index, uint8_t value) {
	if (index < 0 || index >= size) {
		std::cout << "Memory::write(" << index << "," << value << ") out of bounds exception (size = " << size << ")" << std::endl;
		return false;
	}

	data[index] = value;
	return true;
}