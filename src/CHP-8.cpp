/*

CHP-8

*/

#include <iostream>
#include <iomanip>

#include <SFML/Graphics.hpp>

#include "CHP-8.hpp"

/****************************************************************
			Main
****************************************************************/

int main(int argc, char* argv[]) {

	// Determine the ROM
	// TODO

	// Create the chip
	chp8::Chip8 chip("Nothing1", "Nothing2");

	// Loop
	sf::Clock timer;
	while (chip.isActive()) {
		float dt = timer.getElapsedTime().asSeconds();
		timer.restart();
		chip.tick(dt);
		chip.render(dt);
	}

}

/****************************************************************
			Misc
****************************************************************/

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1) {
	static const char* digits = "0123456789ABCDEF";
	std::string rc(hex_len, '0');
	for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
		rc[i] = digits[(w >> j) & 0x0f];
	return rc;
}

using namespace chp8;

/****************************************************************
			Chip8 Class : Constructors/Destructors
****************************************************************/

Chip8::Chip8(std::string conf, std::string romPath) : memory(0x1000) {
	infoWindow.create(sf::VideoMode(280, 320), "CHP-8 INFO");
	infoWindow.setPosition(sf::Vector2i(0,0));
	infoWindow.setFramerateLimit(60);

	if (!font.loadFromFile("CONSOLA.TTF")) {
		// Failed to load, abort
		std::cout << "Failed to load font, aborting" << std::endl;
		chipActive = false; return;
	}

	videoSystem.setVideoMode(MonoVideo::_128x64);

	chipActive = true;
}

/****************************************************************
			Chip8 Class : Rendering and Ticking
****************************************************************/

void Chip8::tick(float dt) {
	instructionsThisTick = 0;

	// Add dt to our accumulator
	timeAccum += dt;

	if (videoTest) {
		if (videoTestMode)
			test_videoInversionPatternTwo();
		else
			test_videoInversionPatternOne();
	}
	else {
		// Check for errors
		if (error != Chip8Error::None) {
			// Exit with errors
			chipActive = false;

			std::cout << "Chip8 Error: ";
			switch (error) {
			case StackOverflow:
				std::cout << "Stack Overflow!" << std::endl;
				break;

			case StackUnderflow:
				std::cout << "Stack Underflow!" << std::endl;
				break;

			case UnknownOpcode:
				std::cout << "Unknown Opcode!" << std::endl;
				break;

			default:
				std::cout << "Unknown Type" << std::endl;
				break;
			}
		}

		while (timeAccum > timePerInstruction) {
			timeAccum -= timePerInstruction;
			instructionsThisTick++;

			// Get the instruction at the current PC
			uint16_t instruction = (memory.read(pc) << 8) | memory.read(pc + 1);

			// Process an instruction
			// Do a top level decision on the first hex digit
			uint8_t digitOne = (instruction & 0xF000) >> 12;

			std::cout << "Instruction 0x" << std::hex << std::setw(4) << instruction << ", first digit: 0x" << std::hex << digitOne << std::endl;

			switch (digitOne) {
			case 0:
				executeFamily0(instruction);
				break;

			case 1:
				executeFamily1(instruction);
				break;

			case 2:
				executeFamily2(instruction);
				break;

			case 3:
				executeFamily3(instruction);
				break;

			case 4:
				executeFamily4(instruction);
				break;

			case 5:
				executeFamily5(instruction);
				break;

			case 6:
				executeFamily6(instruction);
				break;

			case 7:
				executeFamily7(instruction);
				break;

			case 8:
				executeFamily8(instruction);
				break;

			case 9:
				executeFamily9(instruction);
				break;

			case 0xA:
				executeFamilyA(instruction);
				break;

			case 0xB:
				executeFamilyB(instruction);
				break;

			case 0xC:
				executeFamilyC(instruction);
				break;

			case 0xD:
				executeFamilyD(instruction);
				break;

			case 0xE:
				executeFamilyE(instruction);
				break;

			case 0xF:
				executeFamilyF(instruction);
				break;

			default:
				// Flag the unknown opcode error
				error = Chip8Error::UnknownOpcode;
				break;
			}

			pc += 2;
		}
	}

	// Do video ticking
	videoSystem.tick(dt);
	// Check for an inactive videoSystem. If it goes inactive, shutdown
	if (!videoSystem.isActive()) {
		chipActive = false;
	}
}

void Chip8::render(float dt) {
	infoWindow.clear(sf::Color::Black);

	sf::Text drawText("", font, 14);
	float x = 10; float y = 5;

	// Display the timing information
	drawText.setString("dt = " + std::to_string(dt) + " seconds");
	drawText.setPosition(x, y);
	infoWindow.draw(drawText);

	drawText.setString("timeAccum = " + std::to_string(timeAccum) + " seconds");
	drawText.setPosition(x, y += 14);
	infoWindow.draw(drawText);

	drawText.setString("instructionsThisTick: " + std::to_string(instructionsThisTick));
	drawText.setPosition(x, y += 14);
	infoWindow.draw(drawText);

	y = 50;
	// Display the registers
	for (uint8_t i = 0; i < 0x10; i++) {
		drawText.setString("V" + n2hexstr(i, 1) + ": " + n2hexstr(r[i]));
		drawText.setPosition(x, y);
		infoWindow.draw(drawText);
		x += 80;
		if (x >= 100) {
			x = 10;
			y += 16;
		}
	}
	
	drawText.setString("SD: " + n2hexstr(r_sound));
	drawText.setPosition(x = 10, y);
	infoWindow.draw(drawText);

	drawText.setString("DL: " + n2hexstr(r_delay));
	drawText.setPosition(x += 80, y);
	infoWindow.draw(drawText);

	drawText.setString("PC: " + n2hexstr(pc));
	drawText.setPosition(x = 10, y += 16);
	infoWindow.draw(drawText);

	drawText.setString("SP: " + n2hexstr(sp));
	drawText.setPosition(x, y += 16);
	infoWindow.draw(drawText);

	// Display stack
	y += 20;
	for (int i = sp + 3, c = 0; i >= 0 && c < 5; i--) {
		if (i >= 0x10) {
			drawText.setString("Stack @ [" + n2hexstr(i, 1) + "] = X");
		}
		else {
			drawText.setString("Stack @ [" + n2hexstr(i, 1) + "] = " + n2hexstr(stack[i]));
			c++;
		}
		drawText.setPosition(x, y);
		if (i == sp)
			drawText.setFillColor(sf::Color(80, 80, 255));
		else
			drawText.setFillColor(sf::Color::White);
		infoWindow.draw(drawText);
		y += 14;
	}

	// Update window
	infoWindow.display();
}

/****************************************************************
			Chip8 Class : Misc
****************************************************************/

bool Chip8::isActive() {
	return chipActive;
}

/****************************************************************
			Chip8 Class : (Private) Chip Errors
****************************************************************/

bool Chip8::hasErrored() {
	return error != Chip8Error::None;
}

/****************************************************************
			Chip8 Class : Stack
****************************************************************/

void Chip8::pushStack(uint16_t value) {
	if (sp <= 0x0F) {
		// Push to the stack
		stack[sp] = value;
		sp++;
	}
	else {
		// Error!
		error = Chip8Error::StackOverflow;
	}
}

uint16_t Chip8::popStack() {
	sp--;
	if (sp < 0)
		error = Chip8Error::StackUnderflow;
	else
		return stack[sp];
	return 0;
}

uint16_t Chip8::peakStack() {
	if (sp < 0)
		error = Chip8Error::StackUnderflow;
	else if (sp > 0xF)
		error = Chip8Error::StackOverflow;
	else
		return stack[sp];
	return 0;
}

/****************************************************************
			Chip8 Class : Execution
****************************************************************/

void Chip8::executeCall(uint16_t target) {
	// We need to set PC to target - 2. Also push the current PC to the stack, UNCHANGED so when it is
	// popped back with the RET, the auto-increment allows us to execute correct next instruction
	pushStack(pc);
	if (!hasErrored()) {
		pc = target - 2;
	}
	else {
		std::cout << "Call interrupted due to stack error" << std::endl;
	}
}

void Chip8::executeFamily0(uint16_t instruction) {
	uint16_t sVal = (instruction & 0x0FFF); // Get digits: 0XXX
	switch (sVal) {
	case 0x0E0: // CLS: clear the display
		videoSystem.setAllPixels(false); // Set all the pixels to inactive
		break;
		
	case 0x0EE: // RET: return from subroutine
		uint16_t poppedValue = popStack(); // Pop the top value off the stack
		// If we didn't error, set PC to the value we popped off the stack (points to the instruction that jumped, so when the PC is incremented
		// we will be on the correct instruction to continue)
		if (!hasErrored()) {
			pc = poppedValue;
		}
		break;

	default:
		error = Chip8Error::UnknownOpcode;
		break;
	}
}

void Chip8::executeFamily1(uint16_t instruction) {
	// This is just a jump instruction with 0NNN giving the PC to jump to
	// Set the PC to 0NNN - 2, as we need to execute the instruction at 0NNN even after the increment at the end of the cycle
	uint16_t target = instruction & 0x0FFF;
	pc = target - 2;
}

void Chip8::executeFamily2(uint16_t instruction) {
	// This is a call instruction to 0NNN giving the PC to jump to
	// As with 1NNN, 
	uint16_t target = instruction & 0x0FFF;
	executeCall(target);
}

void Chip8::executeFamily3(uint16_t instruction) {
	// Skip next instruction if Vx == kk, (3xkk)
	// To do this skipping, do a pc += 2 in here, combined with the auto-increment
	uint8_t reg = (instruction & 0x0F00) >> 8;
	uint8_t byte = instruction & 0x00FF;
	if (r[reg] == byte) {
		pc += 2;
	}
}

void Chip8::executeFamily4(uint16_t instruction) {
	// Skip next instruction if Vx != kk, (4xkk)
	// To do this skipping, do a pc += 2 in here, combined with the auto-increment
	uint8_t reg = (instruction & 0x0F00) >> 8;
	uint8_t byte = instruction & 0x00FF;
	if (r[reg] != byte) {
		pc += 2;
	}
}

void Chip8::executeFamily5(uint16_t instruction) {
	// Skip next instruction if Vx == Vy, (5xy0)
	// To do this skipping, do a pc += 2 in here, combined with the auto-increment
	uint8_t regA = (instruction & 0x0F00) >> 8;
	uint8_t regB = (instruction & 0x00F0) >> 4;
	if (r[regA] == r[regB]) {
		pc += 2;
	}
}

void Chip8::executeFamily6(uint16_t instruction) {
	// Set Vx = kk, (6xkk)
	uint8_t reg = (instruction & 0x0F00) >> 8;
	uint8_t byte = instruction & 0x00FF;
	r[reg] = byte;
}

void Chip8::executeFamily7(uint16_t instruction) {
	// Set Vx += kk, (7xkk)
	uint8_t reg = (instruction & 0x0F00) >> 8;
	uint8_t byte = instruction & 0x00FF;
	r[reg] += byte;
}

void Chip8::executeFamily8(uint16_t instruction) {
	uint8_t sVal = instruction & 0x000F; // Final hex digit gives the type of instruction
	uint8_t regA = (instruction & 0x0F00) >> 8;
	uint8_t regB = (instruction & 0x00F0) >> 4;

	switch (sVal) {
	case 0:
		// Set Vx = Vy, (8xy0)
		r[regA] = r[regB];
		break;

	case 1:
		// Set Vx = (Vx | Vy), (8xy1)
		r[regA] = r[regA] | r[regB];
		break;

	case 2:
		// Set Vx = (Vx & Vy), (8xy2)
		r[regA] = r[regA] & r[regB];
		break;

	case 3:
		// Set Vx = (Vx ^ Vy), (8xy3)
		r[regA] = r[regA] ^ r[regB];
		break;

	case 4:
		// Set Vx = Vx + Vy, set VF = carry (if Vx + Vy > 255), (8xy4)
		uint16_t result = r[regA] + r[regB];
		if (result > 255)
			r[0xF] = 1;
		else
			r[0xF] = 0;
		r[regA] = (uint8_t)result;
		break;

	case 5:
		// Set Vx = Vx - Vy, set VF = carry (if Vx > Vy), (8xy5)
		if (r[regA] > r[regB])
			r[0xF] = 1;
		else
			r[0xF] = 0;
		r[regA] = (uint8_t)(r[regA] - r[regB]);
		break;

	case 6:
		// Set Vx = Vx >> 1, set VF to (Vx & 0x1) before shift, (8xy6)
		r[0xF] = r[regA] & 0x1;
		r[regA] = r[regA] >> 1;
		break;

	case 7:
		// Set Vx = Vy - Vx, set VF = carry (if Vy > Vx), (8xy7)
		if (r[regB] > r[regA])
			r[0xF] = 1;
		else
			r[0xF] = 0;
		r[regA] = (uint8_t)(r[regB] - r[regA]);
		break;

	case 0xE:
		// Set Vx = Vx << 1, set VF to (Vx & 0x1) before shift, (8xyE)
		r[0xF] = (r[regA] & 0x8000) >> 15;
		r[regA] = r[regA] << 1;
		break;

	default:
		error = Chip8Error::UnknownOpcode;
		break;
	}
	
}

void Chip8::executeFamily9(uint16_t instruction) {
	// Skip next instruction if Vx != Vy, (9xy0)
	// To do this skipping, do a pc += 2 in here, combined with the auto-increment
	uint8_t regA = (instruction & 0x0F00) >> 8;
	uint8_t regB = (instruction & 0x00F0) >> 4;
	if (r[regA] != r[regB]) {
		pc += 2;
	}
}

void Chip8::executeFamilyA(uint16_t instruction) {
	// Puts 0NNN into the register I
	r_I = instruction & 0x0FFF;
}

void Chip8::executeFamilyB(uint16_t instruction) {
	// Jump to V0 + 0NNN
	uint16_t target = r[0x0] + (instruction & 0x0FFF);
	executeCall(target);
}

void Chip8::executeFamilyC(uint16_t instruction) {
	// Vx = random AND kk, (Cxkk)
	uint8_t randomByte = rand() % 256;
	uint8_t kk = instruction & 0x00FF;
	uint8_t regA = (instruction & 0x0F00) >> 8;
	r[regA] = randomByte & kk;
}

void Chip8::executeFamilyD(uint16_t instruction) {
	// TODO
	// Draw a n-byte sprite starting at I, with coordinates starting at (Vx,Vy), VF set if collision
}

void Chip8::executeFamilyE(uint16_t instruction) {
	uint8_t sVal = instruction & 0x00FF; // Final hex digit gives the type of instruction
	uint8_t regA = (instruction & 0x0F00) >> 8;

	switch (sVal) {
	case 0x9E:
		// Skip next instruction if key with value Vx is pressed (to do skip, pc += 2)
		break;

	case 0xA1:
		// Skip next instruction if key with value Vx is not pressed (to do skip, pc += 2)
		break;

	default:
		error = Chip8Error::UnknownOpcode;
		break;
	}
}

void Chip8::executeFamilyF(uint16_t instruction) {

}

/****************************************************************
			Chip8 Class : Video Test
****************************************************************/

void Chip8::test_videoInversionPattern(int xInc, int yInc) {
	videoSystem.invertPixel(ix, iy);
	ix += xInc;
	if (ix >= videoSystem.getMode().width) {
		ix = 0; iy += yInc;
	}
	if (iy >= videoSystem.getMode().height) {
		iy = 0;
		videoSystem.invertAllPixels();
		videoTestMode = !videoTestMode;
	}
}

void Chip8::test_videoInversionPatternOne() {
	if (test_videoInversionPatternClock.getElapsedTime().asMicroseconds() < 200)
		return;

	test_videoInversionPatternClock.restart();
	test_videoInversionPattern(2, 2);
}

void Chip8::test_videoInversionPatternTwo() {
	if (test_videoInversionPatternClock.getElapsedTime().asMicroseconds() < 100)
		return;

	test_videoInversionPatternClock.restart();
	test_videoInversionPattern(1, 3);
}