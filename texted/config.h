#pragma once

namespace
{
	int fontSize = 18;
	int scrollSpeed = 4;

	sf::Color fgColor = sf::Color::White;
	sf::Color bgColor = sf::Color(30, 30, 30, 255);
	sf::Color lineNumColor = sf::Color::Yellow;
	sf::Color lineNumBgColor = sf::Color(50, 50, 50, 255);

	std::string keywords[] = { "while", "for", "if", "return", "break", "continue", "switch", "do" };
	sf::Color kwColor = sf::Color::Magenta;

	std::string datatypes[] = { "int", "bool", "string", "std::string", "vector", "std::vector", "list", "std::list", "char", "float", "double", "void" };
	sf::Color dtColor = sf::Color::Cyan;

	sf::Color strColor = sf::Color(200, 100, 0, 255);
	sf::Color numColor = sf::Color::Yellow;
	sf::Color commentColor = sf::Color::Green;
}