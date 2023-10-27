#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <iostream>
#include "Utils.h"
#include "PlayerBase.h"

class Client
{
public:
	Client();
	~Client();
	bool init();
	bool update(float dt);
protected:
	void handleIncomingData();

	bool requestPlayerData();
	void loadPlayerList(sf::Packet);
	void loadNewConnectedPlayer(sf::Packet);
	void unloadDisconnectedPlayer(sf::Packet);

	sf::IpAddress serverIP;
	int serverPort;
	sf::TcpSocket socket;
	sf::RenderWindow* window;

	std::string myUsername;
	std::vector<PlayerData> players;
	PlayerBase player;
};

