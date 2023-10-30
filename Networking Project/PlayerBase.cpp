#include "PlayerBase.h"

PlayerBase::PlayerBase() {
	speed = 100;
	penguinBase.setSize(sf::Vector2f(100, 120));
	penguinBase.setOrigin(penguinBase.getSize() * 0.5f);
	penguinColour.setSize(sf::Vector2f(100, 120));
	penguinColour.setOrigin(penguinColour.getSize() * 0.5f);
	font.loadFromFile("font/04B_30__.TTF");
	usernameText.setFont(font);
	usernameText.setCharacterSize(15);
}

PlayerBase::~PlayerBase() {

}

void PlayerBase::init(sf::Texture* penguinBaseTex, sf::Texture* penguinColourTex, PlayerData playerData) {
	penguinBase.setTexture(penguinBaseTex);
	penguinColour.setTexture(penguinColourTex);
	data = playerData;
	penguinColour.setFillColor(data.colour);
	usernameText.setString(data.name);
	usernameText.setOrigin(usernameText.getGlobalBounds().getSize() / 2.f + usernameText.getLocalBounds().getPosition());
	usernameText.setFillColor(data.colour);
	setPos(data.x, data.y);
}

void PlayerBase::update(float dt) {
}

void PlayerBase::render(sf::RenderWindow* window) {
	window->draw(penguinBase);
	window->draw(penguinColour);
	window->draw(usernameText);
}

void PlayerBase::setPos(float x, float y) {
	data.x = x;
	data.y = y;
	updatePos();
}

void PlayerBase::updatePos() {
	usernameText.setPosition(data.x, data.y + 70);
	penguinBase.setPosition(data.x, data.y);
	penguinColour.setPosition(data.x, data.y);
}

void PlayerBase::setColour(sf::Color colour) {
	data.colour = colour;
	penguinColour.setFillColor(data.colour);
}

std::string PlayerBase::getName() {
	return data.name;
}

sf::Vector2f PlayerBase::getPos() {
	return sf::Vector2f(data.x, data.y);
}