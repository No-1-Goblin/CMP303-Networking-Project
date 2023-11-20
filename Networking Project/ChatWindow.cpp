#include "ChatWindow.h"

ChatWindow::ChatWindow() {
	font.loadFromFile("font/arial.ttf");
	isChatting = false;
	for (int i = 0; i < 16; i++) {
		sf::Text* message = new sf::Text();
		message->setFont(font);
		message->setCharacterSize(14);
		message->setString("");
		message->setFillColor(sf::Color::Black);
		messages.push_back(message);
	}
	ChatMessageData msg;
	msg.sender = "SERVER";
	msg.colour = sf::Color::White;
	msg.message = "Welcome to the server!";
	addMessage(msg);
	background.setFillColor(sf::Color(0, 0, 0, 100));
	background.setSize(sf::Vector2f(640, 280));
	myMessage = "Press 'T' to chat";
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

ChatMessageData* ChatWindow::update(float dt) {
	ChatMessageData* returnVal = nullptr;
	if (isChatting) {
		bool isShiftDown = (keyboard->isKeyDown(sf::Keyboard::LShift) || keyboard->isKeyDown(sf::Keyboard::RShift));
		if (keyboard->isKeyPressed(sf::Keyboard::Space)) {
			myMessage += char(32);
		}
		if (keyboard->isKeyPressed(sf::Keyboard::Enter)) {
			returnVal = new ChatMessageData();
			returnVal->message = myMessage;
			returnVal->colour = myMessageText.getFillColor();
			returnVal->sender = myName;
			myMessage = "Press 'T' to chat";
			isChatting = false;
		}
		if (keyboard->isKeyPressed(sf::Keyboard::Escape)) {
			myMessage = "Press 'T' to chat";
			isChatting = false;
			return returnVal;
		}
		if (keyboard->isKeyPressed(sf::Keyboard::Backspace)) {
			if (myMessage.size() > 0) {
				myMessage.erase(myMessage.begin() + myMessage.size() - 1);
			}
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
		// Putting a space at the end of a string does funky things for whatever reason
		if (myMessage.size() >= 2) {
			if (myMessage.at(myMessage.size() - 1) < 32) {
				myMessage.erase(myMessage.begin() + (myMessage.size() - 1));
			}
		}
		myMessageText.setString(myMessage);
		while (myMessageText.getLocalBounds().width > 460) {
			myMessage.erase(myMessage.begin() + (myMessage.size()-1));
			myMessageText.setString(myMessage);
		}
	}
	else {
		if (keyboard->isKeyPressed(sf::Keyboard::T)) {
			isChatting = true;
			myMessage = "";
		}
	}
	return returnVal;
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

void ChatWindow::setPlayerData(PlayerData data) {
	myMessageText.setFillColor(data.colour);
	myName = data.name;
}

void ChatWindow::updatePositions() {
	for (int i = 0; i < messages.size(); i++) {
		messages[i]->setPosition(4, 4 + i * 16);
	}
}

bool ChatWindow::isChatFocused() {
	return isChatting;
}