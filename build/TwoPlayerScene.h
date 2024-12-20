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
	// 创建场景
	static Scene* createGameScene();
	// 初始化
	virtual bool init();
	// 移动飞船
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void onMouseMove(EventMouse* event);
	void onMouseDown(EventMouse* event);
	void updateShip1(float t);
	// 创建子弹
	void createShip1Bullet(float time);
	void createShip2Bullet(float time);
	void createStrongerBullet(float);
	// 移动子弹
	void updateBullet1(float time);
	void updateBullet2(float time);
	// 检测碰撞
	void checkCollision(float t);
	// 游戏结束
	void gameOver(float);
	// 获得增益
	void createStrongerBulletImg(float);
	void checkDoGetStrongerBullet(float);
	void stopCheck(float);

	CREATE_FUNC(twoPlayerScene);

private:
	// 飞船
	Ship* ship1;
	Ship* ship2;
	Vector<Ship*> ships;
	EventListenerMouse* mouseListener;
	// 子弹
	Vector<Bullet*> bullets1;
	Vector<Bullet*> bullets2;
	// 增益
	Sprite* strongerBullet;
	int receiver = 0;
	// 胜者
	int winner = 0;
	// 游戏结束
	Label* gameoverLabel;
	bool enableRestart = false;
};

#endif

