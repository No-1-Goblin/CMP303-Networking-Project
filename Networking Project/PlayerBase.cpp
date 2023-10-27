#include "PlayerBase.h"

PlayerBase::PlayerBase() {
	penguinBase.setSize(sf::Vector2f(100, 120));
	penguinBase.setOrigin(penguinBase.getSize() * 0.5f);
	penguinColour.setSize(sf::Vector2f(100, 120));
	penguinColour.setOrigin(penguinColour.getSize() * 0.5f);
	font.loadFromFile("font/BUMBASTIKA.ttf");
	usernameText.setFont(font);
}

PlayerBase::~PlayerBase() {

}

void PlayerBase::init(sf::Texture* penguinBaseTex, sf::Texture* penguinColourTex, PlayerData playerData) {
	penguinBase.setTexture(penguinBaseTex);
	penguinColour.setTexture(penguinColourTex);
	data = playerData;
	penguinColour.setFillColor(data.colour);
	penguinBase.setPosition(data.x, data.y);
	penguinColour.setPosition(data.x, data.y);
}

void PlayerBase::update(float dt) {
}

void PlayerBase::render(sf::RenderWindow* window) {
	window->draw(penguinBase);
	window->draw(penguinColour);
}