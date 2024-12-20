#pragma once
#ifndef __SPRITES_H__
#define __SPRITES_H__

#include "cocos2d.h"

USING_NS_CC;
#define DISPLAY_WIDTH 1600
#define DISPLAY_HEIGHT 900

class Bullet;
class Alien;

class Ship :public Sprite
{
public:
	static Ship* create(const char* filename);
	//static Ship* create(PolygonInfo p);
	Bullet* createBullet(const char* file = "whiteBullet.png");

	const char* filename = "ship1.png";
	int lives = 3; // 生命值
	int moveSpeed = 10; // 移动速度
	bool isMovingUp = false; // 移动标志
	bool isMovingDown = false;
	bool isMovingLeft = false;
	bool isMovingRight = false;
};

class Bullet :public Sprite
{
public:
	static Bullet* create(const char*);
	Bullet(); // 用来设置各个成员变量的值
	virtual bool update(Vec2 Pos = Vec2::ZERO, Vec2 Pos2 = Vec2::ZERO);
	virtual float getShipBulletSpeedY() { return shipBulletSpeedY; }
	float getAlienBulletSpeed() { return alienBulletSpeed; }
	virtual bool hasDirection() { return isBossBulletOne; }
	virtual Vec2 getBeginPos() { return beginPos; }
	virtual Vec2 getTargetPos() { return targetPos; }

	float shipBulletSpeedY = 15.0f; // 飞船子弹的速度
	float alienBulletSpeed = 8.0f; // 外星人子弹的速度
	bool isBossBulletOne = false; // 是否是BossBulletOne，如是则有方向
	// 用于确定子弹发射的方向
	Vec2 beginPos = Vec2::ZERO; // 子弹发射的起点
	Vec2 targetPos = Vec2::ZERO; // 子弹发射的终点
	int label = 0; // 标签，继承类的标签在GameScene中各自的create函数中修改
};

class bossBullet_one :public Bullet
{
public:
	bossBullet_one();
	static bossBullet_one* create(const char*);

};

class Alien :public Sprite
{
public:
	static Alien* create(const char* filename);
	virtual float getSpeedY() { return alienSpeedY; }
	virtual Bullet* createAlienBullet() { return nullptr; }
	virtual bool ifHasBullet() { return hasBullet; }
	virtual int getScore() { return score; }
	Alien();

	int dirX = 0; // 左右移动方向
	float alienSpeedX = 2.0f; // 左右平移速度
	float alienSpeedY = 1.0f; // 上下移动速度
	int score = 10; // 分数
	bool hasBullet = false; // 是否有子弹
	int label = 1; // 标签，据此判断是几号外星人
	//int lives = 1;
};

class Alien_second :public Alien
{
public:
	static Alien_second* create(const char*);
	float getSpeedY() override { return alienSpeedY; }
	int getScore()override { return score; }
	Alien_second(); // 构造函数

};

class Alien_third :public Alien
{
public:
	static Alien_third* create(const char*);
	Bullet* createAlienBullet() override;
	float getSpeedY() override { return alienSpeedY; }
	bool ifHasBullet() override { return hasBullet; }
	int getScore()override { return score; }
	Alien_third(); // 构造函数

};

class Alien_boss :public Alien
{
public:
	static Alien_boss* create(const char*);
	Bullet* createBossBullet_one(const char*);
	Bullet* createBossBullet_two(float);
	float getSpeedY() override { return alienSpeedY; }
	bool ifHasBullet() override { return hasBullet; }
	int getScore()override { return score; }

	float alienSpeedY = 2.0f;
	bool hasBullet = true;
	int score = 500;
	Vec2 beginPos;
	Vec2 targetPos;
	int lives = 100;
};


#endif