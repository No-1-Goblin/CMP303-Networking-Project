#include "Client.h"

Client::Client() {
}

Client::~Client() {
    if (window) {
        window->close();
        delete window;
    }
    socket.disconnect();
}

bool Client::init() {
    std::string userInput = "";
    while (serverIP == sf::IpAddress::None) {
        std::cout << "Enter a valid ip address to connect to (or localhost to connect locally): ";
        std::cin >> userInput;
        if (userInput == "localhost") {
            userInput = sf::IpAddress::LocalHost.toString();
        }
        serverIP = sf::IpAddress(userInput);
    }
    userInput = "";
    while (!isValidPort(userInput)) {
        std::cout << "Enter a valid port number to connect to: ";
        std::cin >> userInput;
    }
    serverPort = atoi(userInput.data());
    socket.setBlocking(true);
    if (socket.connect(serverIP, serverPort) != sf::Socket::Done) {
        std::cout << "Failed to connect to server" << std::endl;
        return false;
    }
    std::cout << "Successfully connected to server" << std::endl;
    bool usernameSuccess = false;
    while (!usernameSuccess) {
        sf::Packet usernamePacket;
        userInput = "";
        while (!isValidUsername(userInput)) {
            std::cout << "Enter a username to connect to server (no spaces): ";
            std::cin >> userInput;
        }
        usernamePacket << userInput;
        if (socket.send(usernamePacket) != sf::Socket::Done) {
            std::cout << "Failed to send username data to server" << std::endl;
            return false;
        }
        sf::Packet returnPacket;
        if (socket.receive(returnPacket) != sf::Socket::Done) {
            std::cout << "Failed to receive data from server" << std::endl;
            return false;
        }
        returnPacket >> usernameSuccess;
        if (!usernameSuccess) {
            std::cout << "Username is invalid or already in use" << std::endl;
        }
    }
    myUsername = userInput;
    socket.setBlocking(false);
    if (!requestPlayerData()) {
        std::cout << "Failed to request player data" << std::endl;
        return false;
    }
    window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Plub Cenguin");
}

bool Client::update(float dt) {
    handleIncomingData();
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();
    }
    if (!window->isOpen()) {
        return false;
    }
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    window->clear();
    window->draw(shape);
    window->display();
    return true;
}

void Client::handleIncomingData() {
    sf::Packet packet;
    sf::Socket::Status status = socket.receive(packet);
    if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
        std::cout << "Disconnected from server" << std::endl;
        system("Pause");
        exit(0);
    }
    if (status == sf::Socket::Done) {
        PacketType type;
        packet >> type;
        switch (type) {
        case PacketType::PLAYERLIST:
            loadPlayerList(packet);
            break;
        case PacketType::CONNECTNOTIFICATION:
            loadNewConnectedPlayer(packet);
            break;
        case PacketType::DISCONNECTNOTIFICATION:
            unloadDisconnectedPlayer(packet);
            break;
        }
    }
}

bool Client::requestPlayerData() {
    std::cout << "Requesting player data" << std::endl;
    sf::Packet outPacket;
    outPacket << PacketType::REQUEST;
    outPacket << RequestType::PLAYERLIST;
    sf::Socket::Status status = socket.send(outPacket);
    if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
        return false;
    }
    return true;
}

void Client::loadPlayerList(sf::Packet packet) {
    std::cout << "Received player data from server" << std::endl;
    sf::Int8 playerCount;
    packet >> playerCount;
    for (int i = 0; i < playerCount; i++) {
        PlayerData player;
        packet >> player;
        std::cout << "Player [" << player.name << "] at x=" << player.x << ", y=" << player.y << std::endl;
    }
}

void Client::loadNewConnectedPlayer(sf::Packet packet) {
    PlayerData player;
    packet >> player;
    std::cout << "Player [" << player.name << "] joined the server!" << std::endl;
    if (player.name != myUsername) {
        for (int i = 0; i < players.size(); i++) {
            if (players[i].name == player.name) {
                players.erase(players.begin() + i);
            }
        }
        players.push_back(player);
    }
}

void Client::unloadDisconnectedPlayer(sf::Packet packet) {
    std::string username;
    packet >> username;
    std::cout << "Player [" << username << "] left the server!" << std::endl;
    for (int i = 0; i < players.size(); i++) {
        if (players[i].name == username) {
            players.erase(players.begin() + i);
        }
    }
}