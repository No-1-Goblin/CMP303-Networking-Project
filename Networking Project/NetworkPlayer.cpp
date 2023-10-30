#include "NetworkPlayer.h"

NetworkPlayer::NetworkPlayer() {
	moving = false;
	moveVec = sf::Vector2f(0, 0);
}

NetworkPlayer::~NetworkPlayer() {

}

void NetworkPlayer::init(sf::Texture* penguinBaseTex, sf::Texture* penguinColourTex, PlayerData playerData) {
	PlayerBase::init(penguinBaseTex, penguinColourTex, playerData);
	MovementData temp;
	temp.name = playerData.name;
	temp.x = playerData.x;
	temp.y = playerData.y;
	latest = temp;
	setLatestData(temp);
}

void NetworkPlayer::update(float dt) {
	sf::Vector2f pos = sf::Vector2f(data.x, data.y);
	sf::Vector2f latestDataPos = sf::Vector2f(latest.x, latest.y);
	if (moving || ((pos != latestDataPos) && !moving)) {
		if (!moving) {
			moveVec = (latestDataPos - pos);
			float len = sqrt(pow(moveVec.x, 2) + pow(moveVec.y, 2));
			moveVec *= (1 / len);
		}
		sf::Vector2f moveVel = moveVec * speed * dt;
		setPos(pos.x + moveVel.x, pos.y + moveVel.y);
		sf::Vector2f diff = getPos() - (latestDataPos);
		if (pow(diff.x, 2) + pow(diff.y, 2) <= 1) {
			setPos(latestDataPos.x, latestDataPos.y);
		}
	}
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
	float len = sqrt(pow(moveVec.x, 2) + pow(moveVec.y, 2));
	moveVec *= (1 / len);
}