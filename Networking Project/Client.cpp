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
    sf::Color requestedColour;
    userInput = "";
    while (!isValidColour(userInput)) {
        std::cout << "Enter a value of red for your penguin (0-255): ";
        std::cin >> userInput;
    }
    requestedColour.r = atoi(userInput.data());
    userInput = "";
    while (!isValidColour(userInput)) {
        std::cout << "Enter a value of green for your penguin (0-255): ";
        std::cin >> userInput;
    }
    requestedColour.g = atoi(userInput.data());
    userInput = "";
    while (!isValidColour(userInput)) {
        std::cout << "Enter a value of blue for your penguin (0-255): ";
        std::cin >> userInput;
    }
    requestedColour.b = atoi(userInput.data());
    bool usernameSuccess = false;
    sf::Packet usernamePacket;
    sf::Packet returnPacket;
    while (!usernameSuccess) {
        usernamePacket.clear();
        returnPacket.clear();
        userInput = "";
        while (!isValidUsername(userInput)) {
            std::cout << "Enter a username to connect to server (no spaces): ";
            std::cin >> userInput;
        }
        usernamePacket << userInput;
        usernamePacket << requestedColour;
        if (socket.send(usernamePacket) != sf::Socket::Done) {
            std::cout << "Failed to send username data to server" << std::endl;
            return false;
        }
        if (socket.receive(returnPacket) != sf::Socket::Done) {
            std::cout << "Failed to receive data from server" << std::endl;
            return false;
        }
        PacketType returnType;
        returnPacket >> returnType;
        returnPacket >> usernameSuccess;
        if (returnType != PacketType::USERNAMERESPONSE) {
            usernameSuccess = false;
        }
        if (!usernameSuccess) {
            std::cout << "Username is invalid or already in use" << std::endl;
        }
    }
    PlayerData myData;
    returnPacket >> myData;
    socket.setBlocking(false);
    if (!requestPlayerData()) {
        std::cout << "Failed to request player data" << std::endl;
        return false;
    }
    window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Plub Cenguin");
    penguinBaseTex.loadFromFile("graphics/BasePenguin.png");
    penguinColourTex.loadFromFile("graphics/PenguinColour.png");
    me.init(&penguinBaseTex, &penguinColourTex, myData);
}

bool Client::update(float dt) {
    if (!windowEvents()) {
        return false;
    }
    handleIncomingData();
    me.update(dt);
    if (me.movedThisFrame()) {
        sendMovementPacket();
    }
    render();
    return true;
}

bool Client::windowEvents() {
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();
    }
    if (!window->isOpen()) {
        return false;
    }
    return true;
}

void Client::handleIncomingData() {
    sf::Packet packet;
    sf::Socket::Status status;
    do {
        packet.clear();
        status = socket.receive(packet);
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
            case PacketType::USERNAMERESPONSE:
                break;
            case PacketType::MOVEMENTDATA:
                updatePlayerPosition(packet);
                break;
            }
        }
    } while (status == sf::Socket::Done);
}

void Client::render() {
    window->clear(sf::Color::White);
    window->setView(sf::View(sf::Vector2f(960, 540), sf::Vector2f(1920, 1080)));
    me.render(window);
    for (int i = 0; i < players.size(); i++) {
        players[i]->render(window);
    }
    window->display();
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
        if (player.name != me.getName()) {
            PlayerBase* penguin = new PlayerBase();
            penguin->init(&penguinBaseTex, &penguinColourTex, player);
            players.push_back(penguin);
        }
    }
}

void Client::loadNewConnectedPlayer(sf::Packet packet) {
    PlayerData player;
    packet >> player;
    std::cout << "Player [" << player.name << "] joined the server!" << std::endl;
    if (player.name != me.getName()) {
        for (int i = 0; i < players.size(); i++) {
            if (players[i]->getName() == player.name) {
                players.erase(players.begin() + i);
            }
        }
        PlayerBase* penguin = new PlayerBase();
        penguin->init(&penguinBaseTex, &penguinColourTex, player);
        players.push_back(penguin);
    }
}

void Client::unloadDisconnectedPlayer(sf::Packet packet) {
    std::string username;
    packet >> username;
    std::cout << "Player [" << username << "] left the server!" << std::endl;
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->getName() == username) {
            players.erase(players.begin() + i);
        }
    }
}

void Client::sendMovementPacket() {
    sf::Packet packet;
    MovementData data;
    sf::Vector2f playerPos = me.getPos();
    data.name = me.getName();
    data.x = playerPos.x;
    data.y = playerPos.y;
    packet << PacketType::MOVEMENTDATA;
    packet << data;
    sf::Socket::Status status;
    status = socket.send(packet);
    if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
        std::cout << "Failed to send movement data to server, hopefully not problematic" << std::endl;
    }
}

void Client::updatePlayerPosition(sf::Packet packet) {
    MovementData data;
    packet >> data;
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->getName() == data.name) {
            players[i]->setPos(data.x, data.y);
        }
    }
}