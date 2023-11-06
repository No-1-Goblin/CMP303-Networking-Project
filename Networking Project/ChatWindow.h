#pragma once
#include "SFML/Graphics.hpp"
#include "Utils.h"
#include "Keyboard.h"

class ChatWindow
{
public:
	ChatWindow();
	~ChatWindow();
	void update(float);
	void render(sf::RenderWindow*);
	void addMessage(ChatMessageData);
	void setKeyboard(Keyboard*);
protected:
	void updatePositions();
	std::vector<sf::Text*> messages;
	sf::Font font;
	sf::RectangleShape background;
	Keyboard* keyboard;
	std::string myMessage;
	sf::Text myMessageText;
	int shiftSymbols[21];
	int symbols[11];
};

