#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <iostream>
#include "Utils.h"
#include "NetworkPlayer.h"
#include "PlayerControllable.h"
#include "ChatWindow.h"
#include "Keyboard.h"

class Client
{
public:
	Client();
	~Client();
	bool init();
	bool update(float dt);
protected:
	bool windowEvents();
	void handleIncomingData();
	void render();
	bool requestPlayerData();
	void loadPlayerList(sf::Packet);
	void loadNewConnectedPlayer(sf::Packet);
	void unloadDisconnectedPlayer(sf::Packet);
	void sendMovementPacket();
	void updatePlayerPosition(sf::Packet);
	void printChatMessage(sf::Packet);
	void sendMessage(ChatMessageData);

	sf::IpAddress serverIP;
	int serverPort;
	sf::TcpSocket socket;
	const int SCREEN_WIDTH = 1920;
	const int SCREEN_HEIGHT = 1080;
	sf::RenderWindow* window;
	bool isFocused;

	std::vector<NetworkPlayer*> players;
	sf::Texture penguinBaseTex;
	sf::Texture penguinColourTex;
	PlayerControllable me;
	ChatWindow chat;
	Keyboard keyboard;
};

