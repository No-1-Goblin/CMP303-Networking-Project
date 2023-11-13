#pragma once
#include "SFML/Graphics.hpp"
#include "Utils.h"
#include "Keyboard.h"

class ChatWindow
{
public:
	ChatWindow();
	~ChatWindow();
	ChatMessageData* update(float);
	void render(sf::RenderWindow*);
	void addMessage(ChatMessageData);
	void setKeyboard(Keyboard*);
	void setPlayerData(PlayerData);
	bool isChatFocused();
protected:
	void updatePositions();
	std::vector<sf::Text*> messages;
	sf::Font font;
	sf::RectangleShape background;
	Keyboard* keyboard;
	std::string myName;
	std::string myMessage;
	sf::Text myMessageText;
	int shiftSymbols[21];
	int symbols[11];
	bool isChatting;
};

