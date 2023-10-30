#include "PlayerControllable.h"

PlayerControllable::PlayerControllable() {
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
			// Intentionally send 1 duplicate packet to signal a stop
			stopped = true;
			moved = true;
		}
		else {
			moved = false;
		}
	}
}

bool PlayerControllable::movedThisFrame() {
	return moved;
}