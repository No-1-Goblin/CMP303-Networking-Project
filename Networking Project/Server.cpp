#include "Server.h"

Server::Server() {
	srand(time(0));
}

Server::~Server() {
	listenerSocket.close();
	for (int i = 0; i < connectionQueue.size(); i++) {
		connectionQueue[i]->disconnect();
	}
}

bool Server::init() {
	std::string userInput = "";
	while (!isValidPort(userInput)) {
		std::cout << "Enter a valid port number to host the server on (or 'any' to use any): ";
		std::cin >> userInput;
		if (userInput == "any") {
			userInput = "0";
		}
	}
	port = atoi(userInput.data());
	listenerSocket.setBlocking(false);
	if (listenerSocket.listen(port) == sf::Socket::Error) {
		std::cout << "Failed to open port" << std::endl;
		return false;
	}
	port = listenerSocket.getLocalPort();
	std::cout << "Successfully opened listener socket on port " << port << std::endl;
	return true;
}

void Server::tick(float dt) {
	handleNewConnections();
	handleConnectionQueue();
	handleIncomingData();
}

void Server::handleNewConnections() {
	sf::TcpSocket* client = new sf::TcpSocket;
	if (listenerSocket.accept(*client) == sf::Socket::Done) {
		client->setBlocking(false);
		connectionQueue.push_back(client);
		std::cout << "New attempt to connect from address: " << client->getRemoteAddress() << ":" << client->getRemotePort() << std::endl;
	}
	else {
		delete client;
	}
}

void Server::handleConnectionQueue() {
	for (int i = 0; i < connectionQueue.size(); i++) {
		sf::Packet packet;
		sf::Socket::Status status = connectionQueue[i]->receive(packet);
		switch (status) {
		case sf::Socket::Disconnected:
			delete connectionQueue[i];
			connectionQueue.erase(connectionQueue.begin() + i);
			i--;
			break;
		case sf::Socket::Error:
			delete connectionQueue[i];
			connectionQueue.erase(connectionQueue.begin() + i);
			i--;
			break;
		case sf::Socket::Done:
			std::string username;
			packet >> username;
			sf::Color colour;
			packet >> colour;
			sf::Packet returnPacket;
			sf::TcpSocket* socket = connectionQueue[i];
			if (isValidUsername(username)) {
				bool exists = false;
				for (int i = 0; i < usernames.size(); i++) {
					if (username == usernames[i]) {
						exists = true;
					}
				}
				if (!exists) {
					players.push_back(std::pair<sf::TcpSocket*, std::string>(socket, username));
					usernames.push_back(username);
					PlayerData data;
					data.name = username;
					data.x = rand() % 1920;
					data.y = rand() % 1080;
					data.colour = colour;
					playerData.push_back(data);
					returnPacket << PacketType::USERNAMERESPONSE;
					returnPacket << true;
					returnPacket << data;
					connectionQueue.erase(connectionQueue.begin() + i);
					i--;
					std::cout << "Player [" << username << "] joined the server!" << std::endl;
					sf::Packet connectNotifyPacket;
					connectNotifyPacket << PacketType::CONNECTNOTIFICATION;
					connectNotifyPacket << data;
					broadcastPacket(connectNotifyPacket, username);
				}
			}
			socket->send(returnPacket);
		}
	}
}

void Server::handleIncomingData() {
	sf::Socket::Status status;
	for (int i = 0; i < players.size(); i++) {
		do {
			sf::TcpSocket* socket = players[i].first;
			std::string username = players[i].second;
			sf::Packet packet;
			status = socket->receive(packet);
			if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
				closeConnection(username);
				break;
			}
			if (status == sf::Socket::Done) {
				PacketType type;
				packet >> type;
				switch (type) {
				case PacketType::REQUEST:
					handleRequest(packet, socket);
					break;
				case PacketType::MOVEMENTDATA:
					handleMovementData(packet);
					break;
				case PacketType::CHATMESSAGE:
					broadcastPacket(packet);
					break;
				}
			}
		} while (status == sf::Socket::Done);
	}
}

void Server::broadcastPacket(sf::Packet packet) {
	for (int i = 0; i < players.size(); i++) {
		players[i].first->send(packet);
	}
}

void Server::broadcastPacket(sf::Packet packet, std::string exception) {
	for (int i = 0; i < players.size(); i++) {
		if (players[i].second != exception) {
			players[i].first->send(packet);
		}
	}
}

void Server::closeConnection(std::string username) {
	std::cout << "Player [" << username << "] disconnected." << std::endl;
	for (int i = 0; i < usernames.size(); i++) {
		if (usernames[i] == username) {
			usernames.erase(usernames.begin() + i);
			break;
		}
	}
	for (int i = 0; i < players.size(); i++) {
		if (players[i].second == username) {
			players[i].first->disconnect();
			players.erase(players.begin() + i);
			break;
		}
	}
	for (int i = 0; i < playerData.size(); i++) {
		if (playerData[i].name == username) {
			playerData.erase(playerData.begin() + i);
			break;
		}
	}
	sf::Packet packet;
	packet << PacketType::DISCONNECTNOTIFICATION;
	packet << username;
	broadcastPacket(packet);
}

void Server::handleRequest(sf::Packet requestPacket, sf::TcpSocket* socket) {
	RequestType type;
	requestPacket >> type;
	sf::Packet outPacket;
	bool success = false;
	switch (type) {
	case RequestType::PLAYERLIST:
		outPacket << PacketType::PLAYERLIST;
		outPacket << sf::Int8(playerData.size());
		for (int i = 0; i < playerData.size(); i++) {
			outPacket << playerData[i];
		}
		success = true;
	}
	if (success) {
		socket->send(outPacket);
	}
}

void Server::handleMovementData(sf::Packet packet) {
	MovementData data;
	packet >> data;
	bool validPlayer = false;
	for (int i = 0; i < playerData.size(); i++) {
		if (data.name == playerData[i].name) {
			playerData[i].x = data.x;
			playerData[i].y = data.y;
			validPlayer = true;
			break;
 		}
	}
	if (validPlayer) {
		sf::Packet outPacket;
		outPacket << PacketType::MOVEMENTDATA;
		outPacket << data;
		broadcastPacket(outPacket, data.name);
	}
}