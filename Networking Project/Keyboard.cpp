#include "Keyboard.h"

Keyboard::Keyboard() {
	for (int i = 0; i < 256; i++) {
		keys[i] = false;
		keysLastFrame[i] = false;
	}
}

Keyboard::~Keyboard() {

}

void Keyboard::update() {
	for (int i = 0; i < 256; i++) {
		keysLastFrame[i] = keys[i];
	}
}

bool Keyboard::isKeyDown(int key) {
	return keys[key];
}

bool Keyboard::isKeyPressed(int key) {
	return (keys[key] && !keysLastFrame[key]);
}

void Keyboard::setKeyDown(int key) {
	keys[key] = true;
}

void Keyboard::setKeyUp(int key) {
	keys[key] = false;
}