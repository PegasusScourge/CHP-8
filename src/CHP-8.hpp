#pragma once
/*

CHP-8

*/

#include <cstdint>
#include <string>

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

#include "MonoVideo.hpp"
#include "Memory.hpp"

namespace chp8 {

	/****************************************************************
			Chip8 Class
	****************************************************************/

	class Chip8 {
	public:
		enum Chip8Error { None, StackUnderflow, StackOverflow, UnknownOpcode };

		Chip8(std::string conf, std::string romPath);

		/*
		Rendering and Ticking
		*/
		void tick(float dt);
		void render(float dt);

		/*
		Misc
		*/
		bool isActive();


	private:
		/*
		16 8-bit registers.
		0 through E are general purpose, F is the flag register
		*/
		uint8_t r[0x10]{};

		/*
		Two specific-purpose registers, Sound and Delay. When non-zero, they are decremented at 60Hz
		*/
		uint8_t r_sound = 0;
		uint8_t r_delay = 0;

		/*
		I register
		*/
		uint16_t r_I = 0;

		/*
		Internal state
		*/
		uint16_t pc = 0; // Program counter
		uint8_t sp = 0; // Stack pointer
		bool chipActive = false;

		/*
		Chip Errors
		*/
		Chip8Error error = Chip8Error::None;

		bool hasErrored();

		/*
		Stack
		*/
		uint16_t stack[0x10]{}; // 16 levels of stack

		void pushStack(uint16_t value);
		uint16_t popStack();
		uint16_t peakStack();

		/*
		Memory
		*/
		mem::Memory memory; // 4KB of memory
		MonoVideo videoSystem; // The mono video system has the VRAM in it

		/*
		Information display
		*/
		sf::Font font;
		sf::RenderWindow infoWindow;

		/*
		Timing
		*/
		float timeAccum = 0;
		float timePerInstruction = 0.01f; // In seconds
		int instructionsThisTick = 0;

		/*
		Execution
		*/
		void executeCall(uint16_t target);

		void executeFamily0(uint16_t instruction);
		void executeFamily1(uint16_t instruction);
		void executeFamily2(uint16_t instruction);
		void executeFamily3(uint16_t instruction);
		void executeFamily4(uint16_t instruction);
		void executeFamily5(uint16_t instruction);
		void executeFamily6(uint16_t instruction);
		void executeFamily7(uint16_t instruction);
		void executeFamily8(uint16_t instruction);
		void executeFamily9(uint16_t instruction);
		void executeFamilyA(uint16_t instruction);
		void executeFamilyB(uint16_t instruction);
		void executeFamilyC(uint16_t instruction);
		void executeFamilyD(uint16_t instruction);
		void executeFamilyE(uint16_t instruction);
		void executeFamilyF(uint16_t instruction);

		/*
		Test functions
		*/
		int ix = 0;
		int iy = 0;
		bool videoTest = false;
		bool videoTestMode = false;
		void test_videoInversionPattern(int xInc, int yInc); sf::Clock test_videoInversionPatternClock;
		void test_videoInversionPatternOne();
		void test_videoInversionPatternTwo();

	};

}
