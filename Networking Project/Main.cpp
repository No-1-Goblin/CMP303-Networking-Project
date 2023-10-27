#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Client.h"
#include "Server.h"

int main()
{
    sf::Clock clock;
    std::string userInput = "";
    while (userInput != "host" && userInput != "join") {
        std::cout << "Would you like to 'host' or 'join'? ";
        std::cin >> userInput;
    }
    bool successfulBoot = false;
    if (userInput == "host") {
        Server* server = nullptr;
        while (!successfulBoot) {
            server = new Server();
            if (server->init()) {
                successfulBoot = true;
            }
            else {
                delete server;
            }
        }
        while (true) {
            server->tick(clock.restart().asMilliseconds());
        }
    }
    else {
        Client* client = nullptr;
        while (!successfulBoot) {
            client = new Client();
            if (client->init()) {
                successfulBoot = true;
            }
            else {
                delete client;
            }
        }
        while (client->update(clock.restart().asSeconds()));
    }
    return 0;
}