#include "Keyboard.h"

Keyboard::Keyboard() {
	// Set all key values to false
	for (int i = 0; i < 256; i++) {
		keys[i] = false;
		keysLastFrame[i] = false;
	}
}

Keyboard::~Keyboard() {

}

// Update what keys were pressed last frame
void Keyboard::update() {
	for (int i = 0; i < 256; i++) {
		keysLastFrame[i] = keys[i];
	}
}

// Returns whether the key was down this frame
bool Keyboard::isKeyDown(int key) {
	return keys[key];
}

// Returns whether the key was pressed this frame
bool Keyboard::isKeyPressed(int key) {
	return (keys[key] && !keysLastFrame[key]);
}

// Set the key to be held down
void Keyboard::setKeyDown(int key) {
	keys[key] = true;
}

// Set the key to no longer be held down
void Keyboard::setKeyUp(int key) {
	keys[key] = false;
}