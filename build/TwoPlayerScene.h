#pragma once
#pragma warning(disable:4244)
#ifndef __TWO_PLAYER_SCENE_H__
#define __TWO_PLAYER_SCENE_H__

#include "cocos2d.h"
#include "Sprites.h"

USING_NS_CC;

class twoPlayerScene :public Layer
{
public:
	// ��������
	static Scene* createGameScene();
	// ��ʼ��
	virtual bool init();
	// �ƶ��ɴ�
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void onMouseMove(EventMouse* event);
	void onMouseDown(EventMouse* event);
	void updateShip1(float t);
	// �����ӵ�
	void createShip1Bullet(float time);
	void createShip2Bullet(float time);
	void createStrongerBullet(float);
	// �ƶ��ӵ�
	void updateBullet1(float time);
	void updateBullet2(float time);
	// �����ײ
	void checkCollision(float t);
	// ��Ϸ����
	void gameOver(float);
	// �������
	void createStrongerBulletImg(float);
	void checkDoGetStrongerBullet(float);
	void stopCheck(float);

	CREATE_FUNC(twoPlayerScene);

private:
	// �ɴ�
	Ship* ship1;
	Ship* ship2;
	Vector<Ship*> ships;
	EventListenerMouse* mouseListener;
	// �ӵ�
	Vector<Bullet*> bullets1;
	Vector<Bullet*> bullets2;
	// ����
	Sprite* strongerBullet;
	int receiver = 0;
	// ʤ��
	int winner = 0;
	// ��Ϸ����
	Label* gameoverLabel;
	bool enableRestart = false;
};

#endif

