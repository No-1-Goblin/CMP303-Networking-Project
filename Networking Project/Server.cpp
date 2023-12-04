#include "Server.h"

Server::Server() {
	// Initialise RNG
	srand(time(0));
}

// Clean up sockets
Server::~Server() {
	listenerSocket.close();
	for (int i = 0; i < connectionQueue.size(); i++) {
		connectionQueue[i]->disconnect();
	}
	for (int i = 0; i < players.size(); i++) {
		players[i].first->disconnect();
	}
}

// Initialise listener socket and bind to port
bool Server::init() {
	// Get a port to host server on
	std::string userInput = "";
	while (!isValidPort(userInput)) {
		std::cout << "Enter a valid port number to host the server on (or 'any' to use any): ";
		std::cin >> userInput;
		if (userInput == "any") {
			userInput = "0";
		}
	}
	port = atoi(userInput.data());
	// Set listener socket to not block
	listenerSocket.setBlocking(false);
	// Begin listening on port
	if (listenerSocket.listen(port) == sf::Socket::Error) {
		std::cout << "Failed to open port" << std::endl;
		return false;
	}
	port = listenerSocket.getLocalPort();
	std::cout << "Successfully opened listener socket on port " << port << std::endl;
	// Add listener socket to the selector
	selector.add(listenerSocket);
	return true;
}

// Main server loop
void Server::tick(float dt) {
	// Use selector to wait for a socket to be ready for data to be read
	if (selector.wait()) {
		if (selector.isReady(listenerSocket)) {
			handleNewConnections();
		}
		else {
			handleConnectionQueue();
			handleIncomingData();
		}
	}
}

// Handle any new connections to the listener socket
void Server::handleNewConnections() {
	sf::TcpSocket* client = new sf::TcpSocket;
	// Attempt to accept connection from new client
	if (listenerSocket.accept(*client) == sf::Socket::Done) {
		// Set the socket to be non-blocking
		client->setBlocking(false);
		// Add the socket to the queue for selecting a username and colour
		connectionQueue.push_back(client);
		// Add the socket to the selector
		selector.add(*client);
		std::cout << "New attempt to connect from address: " << client->getRemoteAddress() << ":" << client->getRemotePort() << std::endl;
	}
	else {
		delete client;
	}
}

// Handle any users who have established a connection, but still need to choose a username and colour
void Server::handleConnectionQueue() {
	for (int i = 0; i < connectionQueue.size(); i++) {
		// Find the ready socket
		if (selector.isReady(*connectionQueue[i])) {
			sf::Packet packet;
			// Check status of the connection
			sf::Socket::Status status = connectionQueue[i]->receive(packet);
			switch (status) {
				// If connection has failed for whatever reason, disconnect client and delete socket
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
				// If successfully received data, read it
			case sf::Socket::Done:
				std::string username;
				packet >> username;
				sf::Color colour;
				packet >> colour;
				sf::Packet returnPacket;
				sf::TcpSocket* socket = connectionQueue[i];
				// Check validity of received username data
				if (isValidUsername(username)) {
					bool exists = false;
					for (int i = 0; i < usernames.size(); i++) {
						if (username == usernames[i]) {
							exists = true;
						}
					}
					// If username is valid and not in use, save player data and pack return packet
					if (!exists) {
						// Push the player to the player list
						players.push_back(std::pair<sf::TcpSocket*, std::string>(socket, username));
						usernames.push_back(username);
						PlayerData data;
						data.name = username;
						data.x = rand() % 1920;
						data.y = rand() % 1080;
						data.colour = colour;
						playerData.push_back(data);
						// Pack return packet
						returnPacket << PacketType::USERNAMERESPONSE;
						returnPacket << true;
						returnPacket << data;
						// Remove from connection queue, since they are now a connected player
						connectionQueue.erase(connectionQueue.begin() + i);
						i--;
						std::cout << "Player [" << username << "] joined the server!" << std::endl;
						// Send connection notification to other players
						sf::Packet connectNotifyPacket;
						connectNotifyPacket << PacketType::CONNECTNOTIFICATION;
						connectNotifyPacket << data;
						broadcastPacket(connectNotifyPacket, username);
					}
				}
				// Send return packet to user
				socket->send(returnPacket);
			}
		}
	}
}

// Handle incoming data from connected players
void Server::handleIncomingData() {
	sf::Socket::Status status;
	for (int i = 0; i < players.size(); i++) {
		// Find socket that has received data
		if (selector.isReady(*players[i].first)) {
			do {
				sf::TcpSocket* socket = players[i].first;
				std::string username = players[i].second;
				sf::Packet packet;
				// Check whether data was successfully received
				status = socket->receive(packet);
				if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
					// Disconnect if connection has failed
					closeConnection(username);
					break;
				}
				// If successful, pass the data to the appropriate handler function
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
}

// Sends the packet to every single player
void Server::broadcastPacket(sf::Packet packet) {
	for (int i = 0; i < players.size(); i++) {
		// Send packet to player
		// We aren't error checking here as any disconnects will be caught in the next main loop iteration
		players[i].first->send(packet);
	}
}

// Sends the packet to every player, except the specified player
void Server::broadcastPacket(sf::Packet packet, std::string exception) {
	for (int i = 0; i < players.size(); i++) {
		// Check player isn't the specified exception
		if (players[i].second != exception) {
			// Send packet to player
			// We aren't error checking here as any disconnects will be caught in the next main loop iteration
			players[i].first->send(packet);
		}
	}
}

// Disconnect the specified player and remove them from any vectors
void Server::closeConnection(std::string username) {
	std::cout << "Player [" << username << "] disconnected." << std::endl;
	// Find player in username list and erase them
	for (int i = 0; i < usernames.size(); i++) {
		if (usernames[i] == username) {
			usernames.erase(usernames.begin() + i);
			break;
		}
	}
	// Find player in player list and close socket, then erase them
	for (int i = 0; i < players.size(); i++) {
		if (players[i].second == username) {
			selector.remove(*players[i].first);
			players[i].first->disconnect();
			players.erase(players.begin() + i);
			break;
		}
	}
	// Find player in playerdata list, and erase them
	for (int i = 0; i < playerData.size(); i++) {
		if (playerData[i].name == username) {
			playerData.erase(playerData.begin() + i);
			break;
		}
	}
	// Send disconnect notification to all players
	sf::Packet packet;
	packet << PacketType::DISCONNECTNOTIFICATION;
	packet << username;
	broadcastPacket(packet);
}

// Handle packets that are of the type "request"
void Server::handleRequest(sf::Packet requestPacket, sf::TcpSocket* socket) {
	RequestType type;
	requestPacket >> type;
	sf::Packet outPacket;
	bool success = false;
	// Check what type of request it is, and prepare the appropriate response packet
	switch (type) {
	case RequestType::PLAYERLIST:
		outPacket << PacketType::PLAYERLIST;
		outPacket << sf::Int8(playerData.size());
		for (int i = 0; i < playerData.size(); i++) {
			outPacket << playerData[i];
		}
		success = true;
	}
	// Send return packet
	if (success) {
		socket->send(outPacket);
	}
}

// Handle movement data received from a player, and forward it to every other player
void Server::handleMovementData(sf::Packet packet) {
	MovementData data;
	packet >> data;
	bool validPlayer = false;
	// Save to the server's copy of player data
	for (int i = 0; i < playerData.size(); i++) {
		if (data.name == playerData[i].name) {
			playerData[i].x = data.x;
			playerData[i].y = data.y;
			validPlayer = true;
			break;
 		}
	}
	// Forward to other players
	if (validPlayer) {
		sf::Packet outPacket;
		outPacket << PacketType::MOVEMENTDATA;
		outPacket << data;
		broadcastPacket(outPacket, data.name);
	}
}