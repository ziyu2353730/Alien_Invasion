#include "cocos2d.h"
#include "Sprites.h"

USING_NS_CC;

Bullet* Bullet::create(const char* filename) // ����ʵ��
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
	shipBulletSpeedY = 15.0f; // �ɴ��ӵ����ٶ�
	alienBulletSpeed = 8.0f; // �������ӵ����ٶ�
	isBossBulletOne = false; // �Ƿ���BossBulletOne���������з���
	// ����ȷ���ӵ�����ķ���
	beginPos = Vec2::ZERO; // �ӵ���������
	targetPos = Vec2::ZERO; // �ӵ�������յ�
	label = 0; // ��ǩ���̳���ı�ǩ��GameScene�и��Ե�create�������޸�
}

bossBullet_one::bossBullet_one()
{
	isBossBulletOne = true;
	// beginPos��targetPos ��GameScene��ceate�������޸�
}

bossBullet_one* bossBullet_one::create(const char* filename) // ����ʵ��
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
