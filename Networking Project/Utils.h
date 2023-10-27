#pragma once
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

enum class PacketType {
	REQUEST,
	PLAYERLIST,
	DISCONNECTNOTIFICATION,
	CONNECTNOTIFICATION,
	USERNAMERESPONSE
};

sf::Packet& operator << (sf::Packet& Packet, const PacketType& p);
sf::Packet& operator >> (sf::Packet& Packet, PacketType& p);

enum class RequestType {
	PLAYERLIST
};

sf::Packet& operator << (sf::Packet& Packet, const RequestType& p);
sf::Packet& operator >> (sf::Packet& Packet, RequestType& p);

struct PlayerData {
	std::string name;
	float x;
	float y;
	sf::Color colour;
};

sf::Packet& operator << (sf::Packet& Packet, const PlayerData& p);
sf::Packet& operator >> (sf::Packet& Packet, PlayerData& p);

sf::Packet& operator << (sf::Packet& Packet, const sf::Color& p);
sf::Packet& operator >> (sf::Packet& Packet, sf::Color& p);


bool isValidPort(std::string);
bool isValidUsername(std::string);
bool isValidColour(std::string);