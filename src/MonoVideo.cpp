#include "MonoVideo.hpp"

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace chp8;

/****************************************************************
			MonoVideo Class Static Defs
****************************************************************/

const MonoVideo::VideoMode MonoVideo::_64x32 = { 64, 32 };
const MonoVideo::VideoMode MonoVideo::_64x48 = { 64, 48 };
const MonoVideo::VideoMode MonoVideo::_64x64 = { 64, 64 };
const MonoVideo::VideoMode MonoVideo::_128x64 = { 128, 64 };

/****************************************************************
			MonoVideo Class
****************************************************************/

MonoVideo::MonoVideo(MonoVideo::VideoMode vmode) {
	// Open the display
	displayWindow.create(sf::VideoMode(512, 256), "CHP-8 MonoVideo Out");
	displayWindow.setPosition(sf::Vector2i(300, 0));
	// displayWindow.setFramerateLimit(60);

	displayActive = true;
	setVideoMode(vmode);
}

void MonoVideo::setVideoMode(MonoVideo::VideoMode vmode) {
	mode = vmode;

	std::cout << "Set MonoVideo vmode to " << mode.width << "x" << mode.height << std::endl;

	// Create the necessary memory
	vram.resize((size_t)mode.width * mode.height);
	std::cout << "VRAM size: " << vram.size() << std::endl;
	// Clear it out
	for (int i = 0; i < vram.size(); i++) {
		vram[i] = false;
	}

	videoBuffer.create(mode.width, mode.height, inactiveColor);
	videoTexture.loadFromImage(videoBuffer);

	std::cout << "videoBuffer " << videoBuffer.getSize().x << "," << videoBuffer.getSize().y << std::endl;
	std::cout << "videoTexture " << videoTexture.getSize().x << "," << videoBuffer.getSize().y << std::endl;

	videoSprite.setTexture(videoTexture, true);
	videoSprite.setScale(2.0f, 2.0f);
	videoSprite.setPosition((displayWindow.getSize().x / 2.0f) - ((videoSprite.getScale().x * mode.width) / 2.0f), (displayWindow.getSize().y / 2.0f) - ((videoSprite.getScale().y * mode.height) / 2.0f));

}

MonoVideo::~MonoVideo() {
	displayWindow.close();
}

void MonoVideo::tick(float dt) {
	sf::Event evt;
	while (displayWindow.pollEvent(evt)) {
		if (evt.type == sf::Event::Closed) {
			// Close the window
			displayWindow.close();
		}
	}

	// Check the window is open before we render
	if (!displayWindow.isOpen()) {
		displayActive = false;
		return;
	}

	// If we have a redraw flag, update the image
	if (redraw) {
		redraw = false;
		for (unsigned int x = 0; x < mode.width; x++)
			for (unsigned int y = 0; y < mode.height; y++)
				videoBuffer.setPixel(x, y, vram[x + (y * mode.width)] ? activeColor : inactiveColor);
	}

	videoTexture.update(videoBuffer);
	displayWindow.draw(videoSprite);

	displayWindow.display();
}

bool MonoVideo::isActive() {
	return displayActive;
}

void MonoVideo::setAllPixels(bool active) {
	for (unsigned int x = 0; x < mode.width; x++)
		for (unsigned int y = 0; y < mode.height; y++)
			setPixel(x, y, active);
}

void MonoVideo::invertAllPixels() {
	for (unsigned int x = 0; x < mode.width; x++)
		for (unsigned int y = 0; y < mode.height; y++)
			invertPixel(x, y);
}

void MonoVideo::setPixel(unsigned int x, unsigned int y, bool a) {
	if (x >= mode.width || y >= mode.height) {
		std::cout << "MonoVideo::setPixel(" << x << "," << y << "," << a << ") out of bounds pixel access" << std::endl;
		return;
	}
	vram[x + (y * mode.width)] = a;
	redraw = true;
}

void MonoVideo::invertPixel(unsigned int x, unsigned int y) {
	if (x >= mode.width || y >= mode.height) {
		std::cout << "MonoVideo::invertPixel(" << x << "," << y << ") out of bounds pixel access" << std::endl;
		return;
	}
		
	vram[x + (y * mode.width)] = !vram[x + (y * mode.width)];
	redraw = true;
}

MonoVideo::VideoMode MonoVideo::getMode() {
	return mode;
}