#include "PlayerBase.h"

PlayerBase::PlayerBase() {

}

PlayerBase::~PlayerBase() {

}

void PlayerBase::init(sf::Texture* penguinBaseTex, sf::Texture* penguinColourTex) {
	penguinBase.setTexture(penguinBaseTex);
	penguinColour.setTexture(penguinColourTex);
}

void PlayerBase::setPlayerColour(sf::Color colour) {
	penguinColour.setFillColor(colour);
}

void PlayerBase::render(sf::RenderWindow* window) {
	window->draw(penguinBase);
	window->draw(penguinColour);
}