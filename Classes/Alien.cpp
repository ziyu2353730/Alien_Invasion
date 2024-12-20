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
	dirX = 0; // �����ƶ�����
	alienSpeedX = 2.0f; // ����ƽ���ٶ�
	alienSpeedY = 1.0f; // �����ƶ��ٶ�
	score = 10; // ����
	hasBullet = false; // �Ƿ����ӵ�
	label = 1; // ��ǩ���ݴ��ж��Ǽ���������
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

Bullet* Alien_third::createAlienBullet() // ����Ҫ���alien3��λ����Ϣ����������Ϊalien_third�ĳ�Ա����
{
	auto bullet = Bullet::create("blueBullet.png");
	auto bulletSize = bullet->getContentSize();

	auto alienSize = this->getContentSize();
	auto alienPos = this->getPosition();
	float bulletPosX = alienPos.x;
	float bulletPosY = alienPos.y - (alienSize.height + bulletSize.height) / 2 - 1;
	bullet->setPosition(bulletPosX, bulletPosY);
	return bullet;
	//this->getParent()->addChild(bullet); ��仰�ᵼ�³������
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

Bullet* Alien_boss::createBossBullet_one(const char* filename) // �������
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

