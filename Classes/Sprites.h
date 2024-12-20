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
	int lives = 3; // ����ֵ
	int moveSpeed = 10; // �ƶ��ٶ�
	bool isMovingUp = false; // �ƶ���־
	bool isMovingDown = false;
	bool isMovingLeft = false;
	bool isMovingRight = false;
};

class Bullet :public Sprite
{
public:
	static Bullet* create(const char*);
	Bullet(); // �������ø�����Ա������ֵ
	virtual bool update(Vec2 Pos = Vec2::ZERO, Vec2 Pos2 = Vec2::ZERO);
	virtual float getShipBulletSpeedY() { return shipBulletSpeedY; }
	float getAlienBulletSpeed() { return alienBulletSpeed; }
	virtual bool hasDirection() { return isBossBulletOne; }
	virtual Vec2 getBeginPos() { return beginPos; }
	virtual Vec2 getTargetPos() { return targetPos; }

	float shipBulletSpeedY = 15.0f; // �ɴ��ӵ����ٶ�
	float alienBulletSpeed = 8.0f; // �������ӵ����ٶ�
	bool isBossBulletOne = false; // �Ƿ���BossBulletOne���������з���
	// ����ȷ���ӵ�����ķ���
	Vec2 beginPos = Vec2::ZERO; // �ӵ���������
	Vec2 targetPos = Vec2::ZERO; // �ӵ�������յ�
	int label = 0; // ��ǩ���̳���ı�ǩ��GameScene�и��Ե�create�������޸�
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

	int dirX = 0; // �����ƶ�����
	float alienSpeedX = 2.0f; // ����ƽ���ٶ�
	float alienSpeedY = 1.0f; // �����ƶ��ٶ�
	int score = 10; // ����
	bool hasBullet = false; // �Ƿ����ӵ�
	int label = 1; // ��ǩ���ݴ��ж��Ǽ���������
	//int lives = 1;
};

class Alien_second :public Alien
{
public:
	static Alien_second* create(const char*);
	float getSpeedY() override { return alienSpeedY; }
	int getScore()override { return score; }
	Alien_second(); // ���캯��

};

class Alien_third :public Alien
{
public:
	static Alien_third* create(const char*);
	Bullet* createAlienBullet() override;
	float getSpeedY() override { return alienSpeedY; }
	bool ifHasBullet() override { return hasBullet; }
	int getScore()override { return score; }
	Alien_third(); // ���캯��

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