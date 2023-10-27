#include "Utils.h"

sf::Packet& operator <<(sf::Packet& Packet, const PacketType& t)
{
    return Packet << static_cast<sf::Int8>(t);
}

sf::Packet& operator >>(sf::Packet& Packet, PacketType& t)
{
    sf::Int8 temp;
    auto ret = Packet >> temp;
    t = (PacketType)temp;
    return ret;
}

sf::Packet& operator <<(sf::Packet& Packet, const RequestType& t)
{
    return Packet << static_cast<sf::Int8>(t);
}

sf::Packet& operator >>(sf::Packet& Packet, RequestType& t)
{
    sf::Int8 temp;
    auto ret = Packet >> temp;
    t = (RequestType)temp;
    return ret;
}

sf::Packet& operator <<(sf::Packet& Packet, const PlayerData& player)
{
    return Packet << player.name << player.x << player.y << player.colour;
}

sf::Packet& operator >>(sf::Packet& Packet, PlayerData& player)
{
    return Packet >> player.name >> player.x >> player.y >> player.colour;
}

sf::Packet& operator <<(sf::Packet& Packet, const sf::Color& colour)
{
    return Packet << colour.r << colour.g << colour.b << colour.a;
}

sf::Packet& operator >>(sf::Packet& Packet, sf::Color& colour)
{
    return Packet >> colour.r >> colour.g >> colour.b >> colour.a;
}

bool isValidPort(std::string port) {
    if (port.length() < 1) {
        return false;
    }
    std::string::iterator it = port.begin();
    while (it != port.end() && std::isdigit(*it)) {
        it++;
    }
    if (it == port.end()) {
        if (atoi(port.data()) >= 0 && atoi(port.data()) <= 65535) {
            return true;
        }
    }
    return false;
}

bool isValidUsername(std::string username) {
    if (username.length() > 0 && username.length() < 16) {
        for (int i = 0; i < username.length(); i++) {
            char c = username.at(i);
            if (!(c >= 34 && c <= 126)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool isValidColour(std::string colour) {
    if (colour.length() > 0 && colour.length() < 4) {
        for (int i = 0; i < colour.size(); i++) {
            char c = colour.at(i);
            if (!isdigit(c)) {
                return false;
            }
        }
        int colourVal = atoi(colour.data());
        if (colourVal >= 0 && colourVal <= 255) {
            return true;
        }
    }
    return false;
}