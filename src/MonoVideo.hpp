#pragma once

#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace chp8 {

	/****************************************************************
			MonoVideo Class
	****************************************************************/

	class MonoVideo {

	public:
		typedef struct {
			size_t width;
			size_t height;
		} VideoMode;
		static const VideoMode _64x32;
		static const VideoMode _64x48;
		static const VideoMode _64x64;
		static const VideoMode _128x64;

		MonoVideo(MonoVideo::VideoMode vmode = MonoVideo::_64x32);
		~MonoVideo();

		void setVideoMode(MonoVideo::VideoMode vmode);

		void tick(float dt);

		bool isActive(); // Returns if the display is active
		MonoVideo::VideoMode getMode();

		void setAllPixels(bool active);
		void invertAllPixels();
		void setPixel(unsigned int x, unsigned int y, bool a);
		void invertPixel(unsigned int x, unsigned int y);

	private:
		VideoMode mode;
		std::vector<bool> vram; // Monochrome video ram array
		bool displayActive;
		bool redraw = true; // Update if the vram buffer has changed state

		sf::Image videoBuffer; // The video buffer
		sf::Texture videoTexture; // The video texture
		sf::Sprite videoSprite;
		sf::Color activeColor = sf::Color::White;
		sf::Color inactiveColor = sf::Color::Black;

		sf::RenderWindow displayWindow;

	};

}