#include "ChatWindow.h"

ChatWindow::ChatWindow() {
	font.loadFromFile("font/arial.ttf");
	for (int i = 0; i < 16; i++) {
		sf::Text* message = new sf::Text();
		message->setFont(font);
		message->setCharacterSize(14);
		message->setString("");
		message->setFillColor(sf::Color::Black);
		messages.push_back(message);
	}
	updatePositions();
	background.setFillColor(sf::Color(0, 0, 0, 100));
	background.setSize(sf::Vector2f(520, 280));
	myMessage = "";
	myMessageText.setFont(font);
	myMessageText.setCharacterSize(14);
	myMessageText.setString(myMessage);
	myMessageText.setFillColor(sf::Color::White);
	myMessageText.setPosition(4, 262);
	shiftSymbols[0] = 41;
	shiftSymbols[1] = 33;
	shiftSymbols[2] = 34;
	// £ is also $ because no extended ASCII allowed
	shiftSymbols[3] = 36;
	shiftSymbols[4] = 36;
	shiftSymbols[5] = 37;
	shiftSymbols[6] = 94;
	shiftSymbols[7] = 38;
	shiftSymbols[8] = 42;
	shiftSymbols[9] = 40;
	shiftSymbols[10] = 123;
	shiftSymbols[11] = 125;
	shiftSymbols[12] = 58;
	shiftSymbols[13] = 60;
	shiftSymbols[14] = 62;
	shiftSymbols[15] = 126;
	shiftSymbols[16] = 63;
	shiftSymbols[17] = 124;
	shiftSymbols[18] = 64;
	shiftSymbols[19] = 43;
	shiftSymbols[20] = 95;

	symbols[0] = 91;
	symbols[1] = 93;
	symbols[2] = 59;
	symbols[3] = 44;
	symbols[4] = 46;
	symbols[5] = 35;
	symbols[6] = 47;
	symbols[7] = 92;
	symbols[8] = 39;
	symbols[9] = 61;
	symbols[10] = 45;
}

ChatWindow::~ChatWindow() {

}

void ChatWindow::update(float dt) {
	bool isShiftDown = (keyboard->isKeyDown(sf::Keyboard::LShift) || keyboard->isKeyDown(sf::Keyboard::RShift));
	if (keyboard->isKeyPressed(sf::Keyboard::Space)) {
		myMessage += " ";
	}
	for (int i = 0; i < 26; i++) {
		int shift = 0;
		if (keyboard->isKeyPressed(i)) {
			if (isShiftDown) {
				shift = 32;
			}
			myMessage += char(97 + i - shift);
		}
	}
	for (int i = 0; i < 10; i++) {
		if (keyboard->isKeyPressed(i + sf::Keyboard::Num0) || keyboard->isKeyPressed(i + sf::Keyboard::Numpad0)) {
			if (!isShiftDown) {
				myMessage += char(48 + i);
			}
			else {
				myMessage += char(shiftSymbols[i]);
			}
		}
	}
	for (int i = 0; i <= 11; i++) {
		if (keyboard->isKeyPressed(sf::Keyboard::LBracket + i)) {
			if (!isShiftDown) {
				myMessage += char(symbols[i]);
			}
			else {
				myMessage += char(shiftSymbols[i + 10]);
			}
		}
	}
	myMessageText.setString(myMessage);
}

void ChatWindow::render(sf::RenderWindow* window) {
	window->draw(background);
	for (int i = 0; i < messages.size(); i++) {
		window->draw(*messages[i]);
	}
	window->draw(myMessageText);
}

void ChatWindow::addMessage(ChatMessageData message) {
	sf::Text* temp = messages[0];
	temp->setString("[" + message.sender + "]: " + message.message);
	temp->setFillColor(message.colour);
	messages.erase(messages.begin());
	messages.push_back(temp);
	updatePositions();
}

void ChatWindow::setKeyboard(Keyboard* ptr) {
	keyboard = ptr;
}

void ChatWindow::updatePositions() {
	for (int i = 0; i < messages.size(); i++) {
		messages[i]->setPosition(4, 4 + i * 16);
	}
}