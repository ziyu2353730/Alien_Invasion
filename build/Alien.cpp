#include "cocos2d.h"
#include "Sprites.h"

USING_NS_CC;

Alien* Alien::create(const char* filename)
{
	auto alien = new Alien();
	if (alien && alien->initWithFile(filename))
	{
		alien->autorelease();
		return alien;
	}
	CC_SAFE_DELETE(alien);
	return nullptr;
}

Alien::Alien()
{
	dirX = 0; // 左右移动方向
	alienSpeedX = 2.0f; // 左右平移速度
	alienSpeedY = 1.0f; // 上下移动速度
	score = 10; // 分数
	hasBullet = false; // 是否有子弹
	label = 1; // 标签，据此判断是几号外星人
}

Alien_second* Alien_second::create(const char* filename)
{
	auto alien = new Alien_second();
	if (alien && alien->initWithFile(filename))
	{
		alien->autorelease();
		return alien;
	}
	CC_SAFE_DELETE(alien);
	return nullptr;
}
Alien_second::Alien_second(): Alien()
{
	alienSpeedY = 3.0f;
	score = 20;
	label = 2;
}

Alien_third* Alien_third::create(const char* filename)
{
	auto alien = new Alien_third();
	if (alien && alien->initWithFile(filename))
	{
		alien->autorelease();
		return alien;
	}
	CC_SAFE_DELETE(alien);
	return nullptr;
}
Alien_third::Alien_third() :Alien()
{
	alienSpeedY = 0.5f;
	hasBullet = true;
	score = 50;
	label = 3;
}

Bullet* Alien_third::createAlienBullet() // 由于要获得alien3的位置信息，将其设置为alien_third的成员函数
{
	auto bullet = Bullet::create("blueBullet.png");
	auto bulletSize = bullet->getContentSize();

	auto alienSize = this->getContentSize();
	auto alienPos = this->getPosition();
	float bulletPosX = alienPos.x;
	float bulletPosY = alienPos.y - (alienSize.height + bulletSize.height) / 2 - 1;
	bullet->setPosition(bulletPosX, bulletPosY);
	return bullet;
	//this->getParent()->addChild(bullet); 这句话会导致程序崩溃
}

Alien_boss* Alien_boss::create(const char* filename)
{
	auto alien = new Alien_boss();
	if (alien && alien->initWithFile(filename))
	{
		alien->autorelease();
		return alien;
	}
	CC_SAFE_DELETE(alien);
	return nullptr;
}

Bullet* Alien_boss::createBossBullet_one(const char* filename) // 设置起点
{
	auto bullet = bossBullet_one::create(filename);
	auto bulletSize = bullet->getContentSize();
	bullet->isBossBulletOne = true;

	auto BossSize = this->getContentSize();
	auto BossPos = this->getPosition();
	float bulletPosX = BossPos.x;
	float bulletPosY = BossPos.y - (BossSize.height + bulletSize.height) / 2 - 1;
	bullet->beginPos = Vec2(bulletPosX, bulletPosY);
	bullet->setPosition(bulletPosX, bulletPosY);
	return bullet;
}

Bullet* Alien_boss::createBossBullet_two(float PosX)
{
	auto bullet = Bullet::create("redBullet.png");
	auto bulletSize = bullet->getContentSize();
	float PosY = DISPLAY_HEIGHT + bulletSize.height / 2;
	bullet->setPosition(PosX, PosY);
	return bullet;
}

