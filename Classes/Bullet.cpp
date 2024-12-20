#include "cocos2d.h"
#include "Sprites.h"

USING_NS_CC;

Bullet* Bullet::create(const char* filename) // 创建实例
{
	auto bullet = new Bullet();
	if (bullet && bullet->initWithFile(filename))
	{
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

Bullet::Bullet()
{
	shipBulletSpeedY = 15.0f; // 飞船子弹的速度
	alienBulletSpeed = 8.0f; // 外星人子弹的速度
	isBossBulletOne = false; // 是否是BossBulletOne，如是则有方向
	// 用于确定子弹发射的方向
	beginPos = Vec2::ZERO; // 子弹发射的起点
	targetPos = Vec2::ZERO; // 子弹发射的终点
	label = 0; // 标签，继承类的标签在GameScene中各自的create函数中修改
}

bossBullet_one::bossBullet_one()
{
	isBossBulletOne = true;
	// beginPos和targetPos 在GameScene的ceate函数中修改
}

bossBullet_one* bossBullet_one::create(const char* filename) // 创建实例
{
	auto bullet = new bossBullet_one();
	if (bullet && bullet->initWithFile(filename))
	{
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

bool Bullet::update(Vec2 Pos, Vec2)
{
	auto curPos = this->getPosition();
	auto bulletSize = this->getContentSize();

	if (curPos.y >= -bulletSize.height / 2 && curPos.x >= 0 && curPos.x <= DISPLAY_WIDTH)
	{
		if (this->hasDirection()) {
			double deltaX = beginPos.x - targetPos.x;
			double deltaY = beginPos.y - targetPos.y;
			double L = sqrt(deltaX * deltaX + deltaY * deltaY);
			curPos.x -= deltaX / L * getAlienBulletSpeed();
			curPos.y -= deltaY / L * getAlienBulletSpeed();
		}
		else
			curPos.y -= this->getAlienBulletSpeed();
		this->setPosition(curPos);
		return true;
	}
	else
		return false;
}
