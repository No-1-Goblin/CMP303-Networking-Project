#pragma once
#include "SFML/Network.hpp"
#include <iostream>
#include "Utils.h"
#include <vector>

class Server
{
public:
	Server();
	~Server();
	bool init();
	void tick(float dt);
protected:
	void handleNewConnections();
	void handleConnectionQueue();
	void handleIncomingData();

	void broadcastPacket(sf::Packet);
	void broadcastPacket(sf::Packet, std::string);
	void closeConnection(std::string);
	void handleRequest(sf::Packet packet, sf::TcpSocket* socket);
	void handleMovementData(sf::Packet);

	// Networking
	uint16_t port;
	sf::TcpListener listenerSocket;
	std::vector<sf::TcpSocket*> connectionQueue;
	sf::SocketSelector selector;

	// Player data
	std::vector<std::string> usernames;
	std::vector<std::pair<sf::TcpSocket*, std::string>> players;
	std::vector<PlayerData> playerData;
};

