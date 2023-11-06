#pragma once
class Keyboard
{
public:
	Keyboard();
	~Keyboard();
	void update();
	bool isKeyDown(int key);
	bool isKeyPressed(int key);
	void setKeyDown(int key);
	void setKeyUp(int key);
protected:
	bool keys[256];
	bool keysLastFrame[256];
};

