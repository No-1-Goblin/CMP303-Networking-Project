#include "PlayerControllable.h"

PlayerControllable::PlayerControllable() {
	// Initialise variables
	stopped = false;
	moved = false;
}

PlayerControllable::~PlayerControllable() {

}

// Handle movement input and move player
void PlayerControllable::update(float dt, bool isFocused) {
	PlayerBase::update(dt);
	if (isFocused) {
		// Get input and set direction
		sf::Vector2f moveDir;
		moveDir.x = ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::D)) - ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::A));
		moveDir.y = ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::S)) - ((int)sf::Keyboard::isKeyPressed(sf::Keyboard::W));
		sf::Vector2f pos = getPos();
		// Move player
		pos += moveDir * speed * dt;
		// Is moving
		if (pos != getPos()) {
			setPos(pos.x, pos.y);
			moved = true;
			stopped = false;
		}
		// Is not moving as of this frame
		else if (!stopped) {
			stopped = true;
			moved = true;
		}
		// Isn't moving and wasn't last frame
		else {
			moved = false;
		}
	}
	// Can't move because window is not focused
	else {
		moved = false;
		stopped = true;
	}
}

// Returns whether the player moved this frame
bool PlayerControllable::movedThisFrame() {
	return moved && !stopped;
}

// Returns whether the player stopped moving this frame
bool PlayerControllable::stoppedThisFrame() {
	return moved && stopped;
}