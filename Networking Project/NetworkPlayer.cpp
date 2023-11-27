#include "NetworkPlayer.h"

NetworkPlayer::NetworkPlayer() {
	// Initialise variables
	moving = false;
	moveVec = sf::Vector2f(0, 0);
}

NetworkPlayer::~NetworkPlayer() {

}

// Initialise with initial data, and bind textures
void NetworkPlayer::init(sf::Texture* penguinBaseTex, sf::Texture* penguinColourTex, PlayerData playerData) {
	// Call init on parent class
	PlayerBase::init(penguinBaseTex, penguinColourTex, playerData);
	// Load player data
	MovementData temp;
	temp.name = playerData.name;
	temp.x = playerData.x;
	temp.y = playerData.y;
	temp.moving = false;
	latest = temp;
	setLatestData(temp);
}

// Update position of the player
void NetworkPlayer::update(float dt) {
	sf::Vector2f pos = sf::Vector2f(data.x, data.y);
	sf::Vector2f latestDataPos = sf::Vector2f(latest.x, latest.y);
	// If the player has moved, or is still moving
	if (moving || ((pos != latestDataPos) && !moving)) {
		// If not moving, then don't overshoot movement
		if (!moving) {
			moveVec = (latestDataPos - pos);
			float len = sqrt(pow(moveVec.x, 2) + pow(moveVec.y, 2));
			moveVec *= (1 / len);
		}
		// Move in the direction of travel that was last received
		sf::Vector2f moveVel = moveVec * speed * dt;
		setPos(pos.x + moveVel.x, pos.y + moveVel.y);
		sf::Vector2f diff = getPos() - (latestDataPos);
		// Stops jittering
		if (pow(diff.x, 2) + pow(diff.y, 2) <= 1) {
			setPos(latestDataPos.x, latestDataPos.y);
		}
	}
}

// Set the latest movement data received about the player
void NetworkPlayer::setLatestData(MovementData newData) {
	prev = latest;
	latest = newData;
	moving = newData.moving;
	// Get direction of travel
	moveVec = sf::Vector2f(latest.x - data.x, latest.y - data.y);
	float len = sqrt(pow(moveVec.x, 2) + pow(moveVec.y, 2));
	moveVec *= (1 / len);
}