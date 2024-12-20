#include "TwoPlayerScene.h"
#include "HelloWorldScene.h"
#include "commonFunc.h"
#include <format> // C++20
#include <string>

#define MOVE_BIAS 10

Scene* twoPlayerScene::createGameScene()
{
	Scene* scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	twoPlayerScene* gameLayer = twoPlayerScene::create();
	scene->addChild(gameLayer);

	return scene;
}

bool twoPlayerScene::init()
{
	if (!Layer::init())
		return false;

	srand(time(NULL));

	ship1 = Ship::create("ship.png");
	ship2 = Ship::create("ship2.png");
	ship1->setPosition(50, DISPLAY_HEIGHT / 2);
	ship2->setPosition(DISPLAY_WIDTH - 50, DISPLAY_HEIGHT / 2);
	ships.pushBack(ship1);
	ships.pushBack(ship2);
	this->addChild(ship1);
	this->addChild(ship2);

	// 设置监听器，读取键盘内容
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(twoPlayerScene::onKeyPressed, this);
	keyListener->onKeyReleased = CC_CALLBACK_2(twoPlayerScene::onKeyReleased, this);
	// 设置监听器，读取鼠标内容
	mouseListener = EventListenerMouse::create();
	mouseListener->onMouseMove = CC_CALLBACK_1(twoPlayerScene::onMouseMove, this);
	mouseListener->onMouseDown = CC_CALLBACK_1(twoPlayerScene::onMouseDown, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
	// 定时器
	this->schedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::updateShip1)); // 更新飞船1移动
	this->schedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::updateBullet1)); // 更新飞船1子弹的位置
	this->schedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::updateBullet2)); // 更新飞船2子弹的位置
	this->schedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::checkCollision)); // 检查子弹是否击中飞船及飞船相撞
	this->scheduleOnce(CC_SCHEDULE_SELECTOR(twoPlayerScene::createStrongerBulletImg), 30.0); // 添加增益
	
	return true;
}

void twoPlayerScene::updateShip1(float deltaTime)
{
	Vec2 shipPosition = ship1->getPosition();
	Vec2 shipSize = ship1->getContentSize();

	if (ship1->isMovingUp && shipPosition.y < DISPLAY_HEIGHT - shipSize.y / 2 - MOVE_BIAS) {
		shipPosition.y += ship1->moveSpeed;
	}
	if (ship1->isMovingDown && shipPosition.y > shipSize.y / 2 + MOVE_BIAS) {
		shipPosition.y -= ship1->moveSpeed;
	}
	if (ship1->isMovingLeft && shipPosition.x > shipSize.x / 2 + MOVE_BIAS) {
		shipPosition.x -= ship1->moveSpeed;
	}
	if (ship1->isMovingRight && shipPosition.x < DISPLAY_WIDTH - shipSize.x / 2 - MOVE_BIAS) {
		shipPosition.x += ship1->moveSpeed;
	}
	ship1->setPosition(shipPosition);
}

void twoPlayerScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	auto scene = HelloWorld::createScene(); // 回溯菜单
	switch (keyCode)
	{
		case EventKeyboard::KeyCode::KEY_ESCAPE:
			Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
			break;

		case EventKeyboard::KeyCode::KEY_W:
			ship1->isMovingUp = true;
			break;
		case EventKeyboard::KeyCode::KEY_A:
			ship1->isMovingLeft = true;
			break;
		case EventKeyboard::KeyCode::KEY_S:
			ship1->isMovingDown = true;
			break;
		case EventKeyboard::KeyCode::KEY_D:
			ship1->isMovingRight = true;
			break;
		case EventKeyboard::KeyCode::KEY_Q:
			ship1->moveSpeed = std::min(15, ship1->moveSpeed + 1);
			break;
		case EventKeyboard::KeyCode::KEY_E:
			ship1->moveSpeed = std::max(5, ship1->moveSpeed - 1);
			break;
		case EventKeyboard::KeyCode::KEY_U:
			if (enableRestart) {
				auto scene = twoPlayerScene::createGameScene();
				Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
			}
			break;
		case EventKeyboard::KeyCode::KEY_SPACE:
			if(!winner)
			{
				if (receiver == 1)
					this->scheduleOnce(CC_SCHEDULE_SELECTOR(twoPlayerScene::createStrongerBullet), 0.0f);
				else
					this->scheduleOnce(CC_SCHEDULE_SELECTOR(twoPlayerScene::createShip1Bullet), 0.0f); // 飞船发射子弹
			}
			break;

		default:
			break;
	}
}

void twoPlayerScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
		case EventKeyboard::KeyCode::KEY_W:
			ship1->isMovingUp = false;
			break;
		case EventKeyboard::KeyCode::KEY_A:
			ship1->isMovingLeft = false;
			break;
		case EventKeyboard::KeyCode::KEY_S:
			ship1->isMovingDown = false;
			break;
		case EventKeyboard::KeyCode::KEY_D:
			ship1->isMovingRight = false;
			break;

		default:
			break;
	}
}

void twoPlayerScene::onMouseMove(EventMouse* event)
{
	auto shipPos = ship2->getPosition();
	auto mousePos = event->getLocation();
	mousePos.y = DISPLAY_HEIGHT - mousePos.y; // 调整
	auto vec = mousePos - shipPos;
	float x = vec.x;
	float y = vec.y;
	float L = sqrtf(x * x + y * y);
	shipPos.x += x / L * ship2->moveSpeed;
	shipPos.y += y / L * ship2->moveSpeed * 1.5;
	//CCLOG("%.2f %.2f", x, y);
	ship2->setPosition(shipPos);
}

void twoPlayerScene::onMouseDown(EventMouse* event)
{
	if (!winner)
	{
		if (receiver == 2)
			this->scheduleOnce(CC_SCHEDULE_SELECTOR(twoPlayerScene::createStrongerBullet), 0.0f);
		else
			this->scheduleOnce(CC_SCHEDULE_SELECTOR(twoPlayerScene::createShip2Bullet), 0.0f); // 飞船发射子弹
	}
}

void twoPlayerScene::createShip1Bullet(float)
{
	auto bullet = Bullet::create("whiteBullet.png");
	auto bulletSize = bullet->getContentSize();
	auto shipSize = ship1->getContentSize();
	auto shipPos = ship1->getPosition();
	float bulletPosX = shipPos.x + shipSize.width / 2;
	float bulletPosY = shipPos.y;
	bullet->setPosition(bulletPosX, bulletPosY);
	bullet->label = 1;
	bullets1.pushBack(bullet);
	this->addChild(bullet);
}

void twoPlayerScene::createShip2Bullet(float)
{
	auto bullet = Bullet::create("blueBullet.png");
	auto bulletSize = bullet->getContentSize();
	auto shipSize = ship2->getContentSize();
	auto shipPos = ship2->getPosition();
	float bulletPosX = shipPos.x - shipSize.width / 2;
	float bulletPosY = shipPos.y;
	bullet->setPosition(bulletPosX, bulletPosY);
	bullet->label = 2;
	bullets2.pushBack(bullet);
	this->addChild(bullet);
}

void twoPlayerScene::createStrongerBullet(float)
{
	Bullet* bullet = bossBullet_one::create("greenBullet.png");
	auto bulletSize = bullet->getContentSize();
	auto ship = receiver == 1 ? ship1 : ship2;
	auto enemy = receiver == 1 ? ship2 : ship1;
	auto shipSize = ship->getContentSize();
	auto shipPos = ship->getPosition();
	float bulletPosX = shipPos.x + shipSize.width / 2;
	float bulletPosY = shipPos.y;
	bullet->setPosition(bulletPosX, bulletPosY);
	bullet->label = 3;
	bullet->beginPos = ship->getPosition();
	bullet->targetPos = enemy->getPosition();
	receiver == 1 ? bullets1.pushBack(bullet) : bullets2.pushBack(bullet);
	this->addChild(bullet);
}

void twoPlayerScene::updateBullet1(float)
{
	for (int i = 0; i < bullets1.size(); i++)
	{
		auto bullet = bullets1.at(i);
		auto bulletPos = bullet->getPosition();
		auto bulletSize = bullet->getContentSize();
		if (bulletPos.x < DISPLAY_WIDTH + bulletSize.width / 2)
		{
			if (bullet->label == 3)
			{
				bullet->update();
			}
			else
			{
				// 上下不移动
				// 一直向右移动
				bulletPos.x += bullet->getShipBulletSpeedY();
				bullet->setPosition(bulletPos);
			}
		}
		else
		{
			bullet->removeFromParentAndCleanup(true);
			bullets1.eraseObject(bullet);
			--i;
		}
	}
}

void twoPlayerScene::updateBullet2(float)
{
	for (int i = 0; i < bullets2.size(); i++)
	{
		auto bullet = bullets2.at(i);
		auto bulletPos = bullet->getPosition();
		auto bulletSize = bullet->getContentSize();
		if (bulletPos.x > - bulletSize.width / 2)
		{
			if (bullet->label == 3)
			{
				auto enemyPos = ship1->getPosition();
				auto beginPos = ship2->getPosition();
				bullet->update(beginPos, enemyPos);
			}
			else
			{
				// 上下不移动
				// 一直向左移动
				bulletPos.x -= bullet->getShipBulletSpeedY();
				bullet->setPosition(bulletPos);
			}
		}
		else
		{
			bullet->removeFromParentAndCleanup(true);
			bullets2.eraseObject(bullet);
			--i;
		}
	}
}

void twoPlayerScene::checkCollision(float)
{
	auto ship1Rect = ship1->getBoundingBox();
	auto ship2Rect = ship2->getBoundingBox();
	for (auto bullet : bullets1)
	{
		auto bulletRect = bullet->getBoundingBox();
		if (ship2Rect.intersectsRect(bulletRect)) {
			winner = 1;
			CCLOG("clicked");
		}
	}
	for (auto bullet : bullets2)
	{
		auto bulletRect = bullet->getBoundingBox();
		if (ship1Rect.intersectsRect(bulletRect)) {
			winner = 2;
			CCLOG("clicked");
		}
	}
	if (ship1Rect.intersectsRect(ship2Rect))
		winner = -1;
	if (winner != 0)
		gameOver(1.0);
}

void twoPlayerScene::gameOver(float)
{
	this->unschedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::updateShip1)); // 更新飞船1移动
	this->unschedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::updateBullet1)); // 更新飞船1子弹的位置
	this->unschedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::updateBullet2)); // 更新飞船2子弹的位置
	this->unschedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::checkCollision)); 
	_eventDispatcher->removeEventListener(mouseListener);
	std::string winnerStr;
	switch (winner)
	{
		case 1:
			winnerStr = "Player one is the WINNER!!!";
			break;
		case 2:
			winnerStr = "Player two is the WINNER!!!";
			break;
		case -1:
			winnerStr = "Draw game!!!";
			break;
	}
	std::string str = std::format("GAME OVER!\n{}\npress 'Esc' to the menu, press 'U' to a new game", winnerStr);

	gameoverLabel = Label::create();
	Vec2 anchorPoint = gameoverLabel->getAnchorPoint();
	Vec2 position = Vec2(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
	initLabelWithText(str, gameoverLabel, Color4B::ORANGE, 50, anchorPoint, position);
	this->addChild(gameoverLabel);

	enableRestart = true;
}

void twoPlayerScene::createStrongerBulletImg(float)
{
	strongerBullet = Sprite::create("strongerBulletOne.png");
	int x = DISPLAY_WIDTH / 2;
	int y = DISPLAY_HEIGHT / 2;
	strongerBullet->setPosition(float(x), float(y));
	this->addChild(strongerBullet);
	this->schedule(CC_SCHEDULE_SELECTOR(twoPlayerScene::checkDoGetStrongerBullet));
}

void twoPlayerScene::checkDoGetStrongerBullet(float)
{
	auto AddUpRect = strongerBullet->getBoundingBox();
	auto ship1Rect = ship1->getBoundingBox();
	auto ship2Rect = ship2->getBoundingBox();
	if (AddUpRect.intersectsRect(ship1Rect)) {
		receiver = 1;
	}
	if (AddUpRect.intersectsRect(ship2Rect)) {
		receiver = 2;
	}
}
