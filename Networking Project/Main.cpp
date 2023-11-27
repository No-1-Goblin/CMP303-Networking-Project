#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Client.h"
#include "Server.h"

int main()
{
    // Initialise clock
    sf::Clock clock;
    // Ask user whether to host server or join server
    std::string userInput = "";
    while (userInput != "host" && userInput != "join") {
        std::cout << "Would you like to 'host' or 'join'? ";
        std::cin >> userInput;
    }
    bool successfulBoot = false;
    // If hosting a server
    if (userInput == "host") {
        Server* server = nullptr;
        // Attempt to boot server
        while (!successfulBoot) {
            server = new Server();
            if (server->init()) {
                successfulBoot = true;
            }
            else {
                delete server;
            }
        }
        // Server main loop
        // Exit code is within server itself
        while (true) {
            server->tick(clock.restart().asMilliseconds());
        }
    }
    // If joining a server
    else {
        Client* client = nullptr;
        // Attempt to join a server
        while (!successfulBoot) {
            client = new Client();
            if (client->init()) {
                successfulBoot = true;
            }
            else {
                delete client;
            }
        }
        // Client main loop
        while (client->update(clock.restart().asSeconds()));
    }
    return 0;
}