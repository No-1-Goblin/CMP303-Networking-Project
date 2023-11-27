#include "Client.h"

Client::Client() {
    isFocused = true;
}

Client::~Client() {
    // Clean up window
    if (window) {
        window->close();
        delete window;
    }
    // Clean up socket
    socket.disconnect();
}

// Attempts to connect to server, and initialises SFML window
bool Client::init() {
    std::string userInput = "";
    // Asks the user for a valid server IP to connect to
    while (serverIP == sf::IpAddress::None) {
        std::cout << "Enter a valid ip address to connect to (or localhost to connect locally): ";
        std::cin >> userInput;
        if (userInput == "localhost") {
            userInput = sf::IpAddress::LocalHost.toString();
        }
        serverIP = sf::IpAddress(userInput);
    }
    userInput = "";
    // Asks the user for a valid port to connect to
    while (!isValidPort(userInput)) {
        std::cout << "Enter a valid port number to connect to: ";
        std::cin >> userInput;
    }
    serverPort = atoi(userInput.data());
    // Set socket to blocking for now until we successfully join server
    // We don't have a window we have to update yet, so we want it to block 
    socket.setBlocking(true);
    // Attempt to establish connection
    if (socket.connect(serverIP, serverPort) != sf::Socket::Done) {
        // If it fails, likely due to invalid IP and/or port, return false which restarts this process
        std::cout << "Failed to connect to server" << std::endl;
        return false;
    }
    std::cout << "Successfully connected to server" << std::endl;
    sf::Color requestedColour;
    userInput = "";
    // Ask the user to choose the colour of their penguin
    // One colour component at a time
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
    // Setup variables and packets for setting username
    bool usernameSuccess = false;
    sf::Packet usernamePacket;
    sf::Packet returnPacket;
    // Repeat until success
    while (!usernameSuccess) {
        // Reset variables
        usernamePacket.clear();
        returnPacket.clear();
        userInput = "";
        // Ask user for a username until a valid one is entered
        while (!isValidUsername(userInput)) {
            std::cout << "Enter a username to connect to server (no spaces): ";
            std::cin >> userInput;
        }
        // Pack the packet to send to server
        usernamePacket << userInput;
        usernamePacket << requestedColour;
        // Attempt to send packet to server
        if (socket.send(usernamePacket) != sf::Socket::Done) {
            // If the connection fails, restart connection process
            std::cout << "Failed to send username data to server" << std::endl;
            return false;
        }
        if (socket.receive(returnPacket) != sf::Socket::Done) {
            // If the connection fails, restart connection process
            // Socket is blocking, so only cause of status not being "Done" is disconnection or errors
            std::cout << "Failed to receive data from server" << std::endl;
            return false;
        }
        // Unpack return packet
        PacketType returnType;
        returnPacket >> returnType;
        returnPacket >> usernameSuccess;
        // If it isn't a usernameresponse packet, something has gone wrong, so try again
        if (returnType != PacketType::USERNAMERESPONSE) {
            usernameSuccess = false;
        }
        // If the server rejects the username, then try again
        if (!usernameSuccess) {
            std::cout << "Username is invalid or already in use" << std::endl;
        }
    }
    // Unpack player data from return packet
    PlayerData myData;
    returnPacket >> myData;
    // Socket needs to no longer be blocking because we will have other stuff going on
    socket.setBlocking(false);
    // Try to request a player list and positions of all players from server
    if (!requestPlayerData()) {
        std::cout << "Failed to request player data" << std::endl;
        return false;
    }
    // Create SFML window
    window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Plub Cenguin");
    window->setFramerateLimit(60);
    // Load textures
    penguinBaseTex.loadFromFile("graphics/BasePenguin.png");
    penguinColourTex.loadFromFile("graphics/PenguinColour.png");
    // Initialise the player, and the chat window
    me.init(&penguinBaseTex, &penguinColourTex, myData);
    chat.setKeyboard(&keyboard);
    chat.setPlayerData(myData);
    // Success, so return true and move to main loop
    return true;
}

// Main loop for client
bool Client::update(float dt) {
    // Handle window events, returns false if window was closed and closes program
    if (!windowEvents()) {
        return false;
    }
    // Handle any received packets
    handleIncomingData();
    // If the chat is not accepting text input, then handle player movement
    if (!chat.isChatFocused()) {
        me.update(dt, isFocused);
        movePacketCooldown -= dt;
        // If the player moved, and hasn't sent a movement packet for a while, send packet
        if (me.movedThisFrame() && movePacketCooldown <= 0) {
            sendMovementPacket(true);
            movePacketCooldown = 0.05;
        }
        // If the player stopped moving, send a stopped movement packet
        // It is important that this gets sent because it tells interpolation calculations to stop predicting movement
        else if (me.stoppedThisFrame()) {
            sendMovementPacket(false);
            movePacketCooldown = 0;
        }
    }
    // Handle movement updates for players other than the one controlled by the user
    for (int i = 0; i < players.size(); i++) {
        players[i]->update(dt);
    }
    // Handle chat updates, and if a message should be sent, send it
    ChatMessageData* msg = chat.update(dt);
    if (msg) {
        sendMessage(*msg);
    }
    // Update the keyboard state
    keyboard.update();
    // Render everything
    render();
    return true;
}

// Handle SFML window events such as closing window, gaining or losing focus, and keyboard presses
bool Client::windowEvents() {
    sf::Event event;
    // Check for window events
    while (window->pollEvent(event))
    {
        switch (event.type) {
            // If closed, close the window and exit the program
        case sf::Event::Closed:
            window->close();
            exit(0);
            break;
            // Update whether the window has focus, used to determine whether to check input
        case sf::Event::GainedFocus:
            isFocused = true;
            break;
        case sf::Event::LostFocus:
            isFocused = false;
            break;
            // Update keyboard states
        case sf::Event::KeyPressed:
            keyboard.setKeyDown(event.key.code);
            break;
        case sf::Event::KeyReleased:
            keyboard.setKeyUp(event.key.code);
            break;
        }
    }
    // If window has been closed, return false to exit program
    if (!window->isOpen()) {
        return false;
    }
    return true;
}

// Handle any incoming packets
void Client::handleIncomingData() {
    sf::Packet packet;
    sf::Socket::Status status;
    // Repeat until all packets are handled
    do {
        // Reset packet
        packet.clear();
        // Read incoming data
        status = socket.receive(packet);
        // If disconnected from server, exit program
        if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
            std::cout << "Disconnected from server" << std::endl;
            window->close();
            system("Pause");
            exit(0);
        }
        // If has received any packets, handle it
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
            case PacketType::CHATMESSAGE:
                printChatMessage(packet);
            }
        }
    } while (status == sf::Socket::Done);
}

// Render everything to the screen
void Client::render() {
    // Reset window
    window->clear(sf::Color::White);
    window->setView(sf::View(sf::Vector2f(960, 540), sf::Vector2f(1920, 1080)));
    // Render the players
    for (int i = 0; i < players.size(); i++) {
        players[i]->render(window);
    }
    me.render(window);
    // Render the chat window
    // The background is transparent, so can be rendered on top
    chat.render(window);
    // Push everything drawn to the screen
    window->display();
}

// Send a request to the server for player data
bool Client::requestPlayerData() {
    std::cout << "Requesting player data" << std::endl;
    sf::Packet outPacket;
    // Pack the packet
    outPacket << PacketType::REQUEST;
    outPacket << RequestType::PLAYERLIST;
    sf::Socket::Status status = socket.send(outPacket);
    // If it fails to send, return false
    if (status == sf::Socket::Error || status == sf::Socket::Disconnected) {
        return false;
    }
    return true;
}

// When a list of player data is received from server, load it and initialise penguins for each
void Client::loadPlayerList(sf::Packet packet) {
    std::cout << "Received player data from server" << std::endl;
    sf::Int8 playerCount;
    // Get playercount from packet
    packet >> playerCount;
    for (int i = 0; i < playerCount; i++) {
        // Unpack a player from the packet
        PlayerData player;
        packet >> player;
        // If it's another player, create a penguin for them
        if (player.name != me.getName()) {
            NetworkPlayer* penguin = new NetworkPlayer();
            penguin->init(&penguinBaseTex, &penguinColourTex, player);
            players.push_back(penguin);
        }
    }
}

// When a new player connects, create a new penguin for them, and push message to chat
void Client::loadNewConnectedPlayer(sf::Packet packet) {
    PlayerData player;
    // Unpack packet
    packet >> player;
    std::cout << "Player [" << player.name << "] joined the server!" << std::endl;
    // Check that somehow a duplicate player join packet hasn't been received
    if (player.name != me.getName()) {
        for (int i = 0; i < players.size(); i++) {
            if (players[i]->getName() == player.name) {
                players.erase(players.begin() + i);
            }
        }
        // Create a new penguin for the connecting player
        NetworkPlayer* penguin = new NetworkPlayer();
        penguin->init(&penguinBaseTex, &penguinColourTex, player);
        players.push_back(penguin);
        // Push a new chat message letting the player know a new player has joined
        ChatMessageData msg;
        msg.sender = "SERVER";
        msg.colour = sf::Color::White;
        msg.message = "Player [" + player.name + "] joined the server!";
        chat.addMessage(msg);
    }
}

// When a disconnect packet is received from server, post chat message to server and get rid of their penguin
void Client::unloadDisconnectedPlayer(sf::Packet packet) {
    std::string username;
    // Unpack packet
    packet >> username;
    std::cout << "Player [" << username << "] left the server!" << std::endl;
    for (int i = 0; i < players.size(); i++) {
        // Find matching player
        if (players[i]->getName() == username) {
            // Post message to chat
            ChatMessageData msg;
            msg.sender = "SERVER";
            msg.colour = sf::Color::White;
            msg.message = "Player [" + username + "] left the server!";
            chat.addMessage(msg);
            // Remove appropriate penguin from player vector
            players.erase(players.begin() + i);
        }
    }
}

// Sends a movement packet to the server, also tells whether the player is still moving or has stopped
void Client::sendMovementPacket(bool moving) {
    sf::Packet packet;
    MovementData data;
    // Pack packet with movement data
    sf::Vector2f playerPos = me.getPos();
    data.name = me.getName();
    data.x = playerPos.x;
    data.y = playerPos.y;
    data.moving = moving;
    // Pack packet
    packet << PacketType::MOVEMENTDATA;
    packet << data;
    sf::Socket::Status status;
    // Send packet
    status = socket.send(packet);
    // If packet fails to send, post error message but this might not be catastrophic,
    // If it is, will be handled at next cycle through main loop anyway
    if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
        std::cout << "Failed to send movement data to server, hopefully not problematic" << std::endl;
    }
}

// When movement data is received, pass it to appropriate penguin
void Client::updatePlayerPosition(sf::Packet packet) {
    MovementData data;
    // Unpack packet
    packet >> data;
    for (int i = 0; i < players.size(); i++) {
        // Find matching player
        if (players[i]->getName() == data.name) {
            // Pass data to penguin
            players[i]->setLatestData(data);
            //std::cout << "Received movement data for player " << data.name << ". Pos x = " << data.x << " y = " << data.y << ", moving = " << data.moving << std::endl;
        }
    }
}

// When chat message packet is received, pass it to the chat window for display
void Client::printChatMessage(sf::Packet packet) {
    ChatMessageData data;
    packet >> data;
    chat.addMessage(data);
}

// Send message data to server
void Client::sendMessage(ChatMessageData msg) {
    sf::Packet packet;
    // Pack packet
    packet << PacketType::CHATMESSAGE;
    packet << msg;
    // Send packet
    sf::Socket::Status status = socket.send(packet);
    // If chat message fails to send, post error message
    // If error is catastrophic, will be handled at next main loop cycle
    if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
        std::cout << "Failed to send chat message" << std::endl;
    }
}