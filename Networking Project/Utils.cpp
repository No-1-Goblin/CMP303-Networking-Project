#include "Utils.h"

// Operator override for packing the PacketType enum
sf::Packet& operator <<(sf::Packet& Packet, const PacketType& t)
{
    return Packet << static_cast<sf::Int8>(t);
}

// Operator override for unpacking the PacketType enum
sf::Packet& operator >>(sf::Packet& Packet, PacketType& t)
{
    sf::Int8 temp;
    auto ret = Packet >> temp;
    t = (PacketType)temp;
    return ret;
}

// Operator override for packing the RequestType enum
sf::Packet& operator <<(sf::Packet& Packet, const RequestType& t)
{
    return Packet << static_cast<sf::Int8>(t);
}

// Operator override for unpacking the RequestType enum
sf::Packet& operator >>(sf::Packet& Packet, RequestType& t)
{
    sf::Int8 temp;
    auto ret = Packet >> temp;
    t = (RequestType)temp;
    return ret;
}

// Operator override for packing the PlayerData struct
sf::Packet& operator <<(sf::Packet& Packet, const PlayerData& player)
{
    return Packet << player.name << player.x << player.y << player.colour;
}

// Operator override for unpacking the PlayerData struct
sf::Packet& operator >>(sf::Packet& Packet, PlayerData& player)
{
    return Packet >> player.name >> player.x >> player.y >> player.colour;
}

// Operator override for packing an sf::Color
sf::Packet& operator <<(sf::Packet& Packet, const sf::Color& colour)
{
    return Packet << colour.r << colour.g << colour.b << colour.a;
}

// Operator override for unpacking an sf::Color
sf::Packet& operator >>(sf::Packet& Packet, sf::Color& colour)
{
    return Packet >> colour.r >> colour.g >> colour.b >> colour.a;
}

// Operator override for packing the MovementData struct
sf::Packet& operator <<(sf::Packet& Packet, const MovementData& data)
{
    return Packet << data.name << data.x << data.y << data.moving << data.id;
}

// Operator override for unpacking the MovementData struct
sf::Packet& operator >>(sf::Packet& Packet, MovementData& data)
{
    return Packet >> data.name >> data.x >> data.y >> data.moving >> data.id;
}

// Operator override for packing the ChatMessageData struct
sf::Packet& operator <<(sf::Packet& Packet, const ChatMessageData& data)
{
    return Packet << data.sender << data.message << data.colour;
}

// Operator override for unpacking the ChatMessageData struct
sf::Packet& operator >>(sf::Packet& Packet, ChatMessageData& data)
{
    return Packet >> data.sender >> data.message >> data.colour;
}

// Checks whether the inputted string is a valid port to connect to
bool isValidPort(std::string port) {
    // Check if there's any text
    if (port.length() < 1) {
        return false;
    }
    // Check each character is a number
    std::string::iterator it = port.begin();
    while (it != port.end() && std::isdigit(*it)) {
        it++;
    }
    if (it == port.end()) {
        // Check the number is a number between 0 and 65335
        if (atoi(port.data()) >= 0 && atoi(port.data()) <= 65535) {
            return true;
        }
    }
    return false;
}

// Checks that an inputted username is valid
bool isValidUsername(std::string username) {
    // Check length of username
    if (username.length() > 0 && username.length() < 16) {
        for (int i = 0; i < username.length(); i++) {
            // Check each character is a letter, number, or symbol
            char c = username.at(i);
            if (!(c >= 34 && c <= 126)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// Check that a colour value is a valid value
bool isValidColour(std::string colour) {
    // Check it is 1-3 characters long
    if (colour.length() > 0 && colour.length() < 4) {
        for (int i = 0; i < colour.size(); i++) {
            char c = colour.at(i);
            // Check each character is a number
            if (!isdigit(c)) {
                return false;
            }
        }
        // Check the number is between 0 and 255
        int colourVal = atoi(colour.data());
        if (colourVal >= 0 && colourVal <= 255) {
            return true;
        }
    }
    return false;
}