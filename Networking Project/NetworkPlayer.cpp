#include "NetworkPlayer.h"

NetworkPlayer::NetworkPlayer() {
	moving = false;
	moveVec = sf::Vector2f(0, 0);
}

NetworkPlayer::~NetworkPlayer() {

}

void NetworkPlayer::update(float dt) {
	sf::Vector2f pos = sf::Vector2f(data.x, data.y);
	sf::Vector2f latestDataPos = sf::Vector2f(latest.x, latest.y);
	if (moving || ((pos != latestDataPos) && !moving)) {
		sf::Vector2f moveVel = moveVec * speed * dt;
	}
	setPos(latest.x, latest.y);
}

void NetworkPlayer::setLatestData(MovementData newData) {
	prev = latest;
	latest = newData;
	if (prev.x == latest.x && prev.y == latest.y) {
		moving = false;
	}
	else {
		moving = true;
	}
	moveVec = sf::Vector2f(latest.x - data.x, latest.y - data.y);
}