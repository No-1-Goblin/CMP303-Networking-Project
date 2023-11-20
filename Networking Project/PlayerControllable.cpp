#include "PlayerControllable.h"

PlayerControllable::PlayerControllable() {
	stopped = false;
	moved = false;
}

PlayerControllable::~PlayerControllable() {

}

void PlayerControllable::update(float dt, bool isFocused) {
	PlayerBase::update(dt);
	if (isFocused) {
		sf::Vector2f moveDir;
		moveDir.x = ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::D)) - ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::A));
		moveDir.y = ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::S)) - ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::W));
		sf::Vector2f pos = getPos();
		pos += moveDir * speed * dt;
		if (pos != getPos()) {
			setPos(pos.x, pos.y);
			moved = true;
			stopped = false;
		}
		else if (!stopped) {
			stopped = true;
			moved = true;
		}
		else {
			moved = false;
		}
	}
	else {
		moved = false;
		stopped = true;
	}
}

bool PlayerControllable::movedThisFrame() {
	return moved && !stopped;
}

bool PlayerControllable::stoppedThisFrame() {
	return moved && stopped;
}