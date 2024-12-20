#include "cocos2d.h"
#include "Sprites.h"

USING_NS_CC;

Ship* Ship::create(const char* filename)
{
	auto ship = new Ship();
	if (ship && ship->initWithFile(filename))
	{
		ship->autorelease();
		return ship;
	}
	CC_SAFE_DELETE(ship);
	return nullptr;
}

Bullet* Ship::createBullet(const char* file)
{
	auto bullet = Bullet::create(file);
	auto bulletSize = bullet->getContentSize();

	auto shipSize = this->getContentSize();
	auto shipPos = this->getPosition();
	float bulletPosX = shipPos.x;
	float bulletPosY = shipPos.y + (shipSize.height + bulletSize.height) / 2 + 1;
	bullet->setPosition(bulletPosX, bulletPosY);
	return bullet;
}