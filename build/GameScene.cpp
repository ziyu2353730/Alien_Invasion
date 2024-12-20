#include <fstream>
#include <string>
#include "GameScene.h"
#include "HelloWorldScene.h"
#include "commonFunc.h"

Scene* GameScene::createGameScene()
{
	Scene* scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	GameScene* gameLayer = GameScene::create();
	scene->addChild(gameLayer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
		return false;

	auto windowSize = getContentSize();
	srand(time(NULL));

	//���ñ������ֲ����ϲ�����
	bgmAudio->playBackgroundMusic(bgmPath1, true);
	// �Ƿ񲥷ű������ֵ�ָʾ��
	bgmItem = MenuItemImage::create(
		"playMusic.png", // ����״̬����ʾ��ͼƬ
		"stopMusic.png", // ѡ��״̬����ʾ��ͼƬ
		CC_CALLBACK_1(GameScene::playBGM, this)
	);
	bgmItem->setAnchorPoint(Vec2(1, 1));
	bgmItem->setPosition(Vec2(windowSize.width - 50, windowSize.height - 100));
	// �Ƿ񲥷���Ч����ʾ��
	effectItem = MenuItemImage::create(
		"playEffect.png", // ����״̬����ʾ��ͼƬ
		"stopEffect.png", // ѡ��״̬����ʾ��ͼƬ
		CC_CALLBACK_1(GameScene::playEffect, this)
	);
	effectItem->setAnchorPoint(Vec2(1, 1));
	effectItem->setPosition(Vec2(windowSize.width - 50, windowSize.height - 165));
	// Menu
	auto itemMenu = Menu::create(bgmItem, effectItem, nullptr);
	itemMenu->setPosition(Vec2::ZERO);
	this->addChild(itemMenu);
	// �����ɴ�
	ship = Ship::create("ship1.png");
	// ����Boss
	boss = Alien_boss::create("Boss.png");
	// ���ݶ���������÷ɴ������boss����
	if (loadData())
		;
	else
	{
		ship->lives = 3;
		curScore = 0;
		ship->setPosition(windowSize.width / 2, 40);
		boss->setPosition(windowSize.width / 2, windowSize.height + 160);
	}
	this->addChild(ship);
	this->addChild(boss);
	// �����ơ�����ǰ����
	scoreLabel = Label::create();
	std::string curStr = "current score : " + std::to_string(curScore);
	initLabelWithText(curStr, scoreLabel, Color4B(255, 0, 0, 122),
		30, Vec2(1, 1), Vec2(windowSize.width - 50, windowSize.height - 50));
	this->addChild(scoreLabel);
	// �����ơ�����ʷ��߷���
	std::string highScorePath = FileUtils::getInstance()->fullPathForFilename("highScore.txt");
	std::string highStr = readFile(highScorePath);
	highScore = stoi(highStr);
	highScoreLabel = Label::create();
	highStr = "highest score :  " + std::to_string(highScore);
	initLabelWithText(highStr, highScoreLabel, Color4B(255, 0, 0, 122),
		30, Vec2(0, 1), Vec2(50, windowSize.height - 50));
	this->addChild(highScoreLabel);
	// ��ʾ�ơ���ʣ������ֵ
	livesLabel = Label::create();
	char str[30];
	sprintf(str, "remanet lives :  %d", ship->lives);
	initLabelWithText(str, livesLabel, Color4B(0, 0, 255, 200),
		30, Vec2(0, 0), Vec2(50, 50));
	this->addChild(livesLabel);

	///////////�����Ƕ���ϷԪ�ص��˶�����
	// �ƶ��ɴ�
	auto listener = EventListenerKeyboard::create();
	// ���ð��¼����ͷż��Ļص�
	listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	// ����������ӵ��¼��ַ���
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	// ��ʱ��
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateShip)); // ���·ɴ��ƶ�
	//Director::getInstance()->
	if(!isBossChapter)
	{
		this->schedule([this](float) {
			this->createAlien("alien1.png");
			}, createAlienFirstInterval, "create_alien_first_key"); // ��ʱ����һ��һ��������
		float t1 = gameTime >= 6.0f ? 0.0f : 6.0f - gameTime;
		float t2 = gameTime >= 8.0f ? 0.0f : 8.0f - gameTime;
		this->scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::startCreateAlienSecond), t1);// 6���ʼ��������������
		this->scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::startCreateAlienThird), t2); // 8���ʼ��������������
	}
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlien)); // ���������˵�λ��
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateBullet)); // ���·ɴ��ӵ���λ��
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet), alienThirdShootInterval); // ���������������ӵ�
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlienBullet)); // �����������ӵ���λ��
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndShip)); // ����������Ƿ���зɴ�
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_ShipAndAlien)); // ���ɴ�����������ײ
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndAlien)); // ����ӵ�����������ײ
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateGameTime)); // ������Ϸ�Ѷ�
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createAddLivesImg), 60.0f); // ��������ֵ
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkDoAddLives));
	return true;
}

void GameScene::updateShip(float deltaTime)
{
	Vec2 shipPosition = ship->getPosition();
	Vec2 shipSize = ship->getContentSize();

	if (ship->isMovingUp && shipPosition.y < DISPLAY_HEIGHT - shipSize.y / 2 - MOVE_BIAS) {
		shipPosition.y += ship->moveSpeed;
	}
	if (ship->isMovingDown && shipPosition.y > shipSize.y / 2 + MOVE_BIAS) {
		shipPosition.y -= ship->moveSpeed;
	}
	if (ship->isMovingLeft && shipPosition.x > shipSize.x / 2 + MOVE_BIAS) {
		shipPosition.x -= ship->moveSpeed;
	}
	if (ship->isMovingRight && shipPosition.x < DISPLAY_WIDTH - shipSize.x / 2 - MOVE_BIAS) {
		shipPosition.x += ship->moveSpeed;
	}
	ship->setPosition(shipPosition);
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	auto scene = HelloWorld::createScene(); // ���ݲ˵�
	switch (keyCode)
	{
		case EventKeyboard::KeyCode::KEY_ESCAPE:
			saveData();
			bgmAudio->pauseBackgroundMusic();
			Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
			break;

		case EventKeyboard::KeyCode::KEY_W:
			ship->isMovingUp = true;
			break;
		case EventKeyboard::KeyCode::KEY_A:
			ship->isMovingLeft = true;
			break;
		case EventKeyboard::KeyCode::KEY_S:
			ship->isMovingDown = true;
			break;
		case EventKeyboard::KeyCode::KEY_D:
			ship->isMovingRight = true;
			break;
		case EventKeyboard::KeyCode::KEY_Q:
			ship->moveSpeed = std::min(15, ship->moveSpeed + 1);
			break;
		case EventKeyboard::KeyCode::KEY_E:
			ship->moveSpeed = std::max(5, ship->moveSpeed - 1);
			break;

		case EventKeyboard::KeyCode::KEY_SPACE:
			if (!enableRestart)
				this->scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::createShipBullet), 0.0f); // �ɴ������ӵ�
			break;
		case EventKeyboard::KeyCode::KEY_U:
			if (enableRestart) {
				bgmAudio->pauseBackgroundMusic();
				saveData();
				auto scene = GameScene::createGameScene();
				Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
			}

		default:
			break;
	}
}
void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
		case EventKeyboard::KeyCode::KEY_W:
			ship->isMovingUp = false;
			break;
		case EventKeyboard::KeyCode::KEY_A:
			ship->isMovingLeft = false;
			break;
		case EventKeyboard::KeyCode::KEY_S:
			ship->isMovingDown = false;
			break;
		case EventKeyboard::KeyCode::KEY_D:
			ship->isMovingRight = false;
			break;

		default:
			break;
	}
}

void GameScene::createShipBullet(float)
{
	auto bullet = ship->createBullet();
	shipBullets.pushBack(bullet);
	this->addChild(bullet);
}

void GameScene::createAlien(const char* filename)
{
	Alien* alien;
	int label = 1;
	if (!std::strcmp("alien2.png", filename)){
		alien = Alien_second::create(filename);
		label = 2;
	}
	else if (!std::strcmp("alien3.png", filename)){
		alien = Alien_third::create(filename);
		label = 3;
	}
	else {
		alien = Alien::create(filename);
	}
	auto alienSize = alien->getContentSize();
	int minX = alienSize.width / 2 + MOVE_BIAS;
	int maxX = DISPLAY_WIDTH - alienSize.width / 2 - MOVE_BIAS;
	int iPosX = rand() % (maxX - minX + 1) + minX;
	int iPosY = DISPLAY_HEIGHT + alienSize.height / 2 + 1;

	if (label == 1){
		// ��������������ƶ�����
		int i = rand() % 2 == 0 ? 0 : 1;
		int j = i == 0 ? 1 : -1;
		alien->dirX = rand() * j % (-2);
	}

	alien->setPosition(Vec2(iPosX, iPosY));
	aliens.pushBack(alien);
	this->addChild(alien);
}


void GameScene::updateAlien(float time)
{
	gameTime += time;
	int timeINT = int(gameTime);
	for (int i = 0; i < aliens.size(); i++)
	{
		auto alien = aliens.at(i);
		if (!alien)
			break;
		auto alienPos = alien->getPosition();
		auto alienSize = alien->getContentSize();
		// ���������ƶ�
		if (alienPos.x < alienSize.width / 2 + MOVE_BIAS ||
			alienPos.x > DISPLAY_WIDTH - alienSize.width / 2 - MOVE_BIAS)
			alien->dirX *= -1; // ���ڷ�����
		alienPos.x += alien->alienSpeedX * alien->dirX;
		// ���������ƶ�
		alienPos.y -= alien->getSpeedY();
		alien->setPosition(alienPos.x, alienPos.y);
		
		if (alienPos.y <= -alienSize.height / 2)
		{
			alien->removeFromParentAndCleanup(true);
			aliens.eraseObject(alien);
			ship->lives--;
			updateLivesLabel();
			if (ship->lives == 0) {
				gameOver();
			}
			--i;
		}
	}
}

void GameScene::createAlienBullet(float)
{
	for (int i = 0; i < aliens.size(); i++)
	{
		auto alien = aliens.at(i);
		if (alien->ifHasBullet()) {
			auto bullet = alien->createAlienBullet();
			this->alienBullets.pushBack(bullet);
			this->addChild(bullet);
		}
	}
}

//void GameScene::updateAlien

void GameScene::updateAlienBullet(float)
{
	for (int i = 0; i < alienBullets.size(); i++)
	{
		auto bullet = this->alienBullets.at(i);
		if (!bullet)
			break; // ��ȫ���ж�
		bool doUpdate = bullet->update();	
		if (!doUpdate) {
			bullet->removeFromParentAndCleanup(true);
			this->alienBullets.eraseObject(bullet);
			--i;
		}
	}
}

void GameScene::updateBullet(float t)
{
	for (int i = 0; i < shipBullets.size(); i++)
	{
		auto bullet = shipBullets.at(i);
		if (!bullet)
			break; // ��ȫ���ж�
		auto bulletPos = bullet->getPosition();
		auto bulletSize = bullet->getContentSize();
		// ���Ҳ��ƶ�
		// һֱ�����ƶ�
		if (bulletPos.y < DISPLAY_HEIGHT + bulletSize.height / 2)
		{
			bulletPos.y += bullet->getShipBulletSpeedY();
			bullet->setPosition(bulletPos);
		}
		else
		{
			bullet->removeFromParentAndCleanup(true);
			shipBullets.eraseObject(bullet);
			--i;
		}
	}
}

void GameScene::checkCollision_BulletAndAlien(float t)
{
	for (int i = 0; i < shipBullets.size(); i++)
	{
		auto bullet = shipBullets.at(i);
		auto bulletRect = bullet->getBoundingBox();
		for (int j = 0; j < aliens.size(); j++) // ����ӵ��������˵���ײ
		{
			auto alien = aliens.at(j);
			auto alienRect = alien->getBoundingBox();
			if (bulletRect.intersectsRect(alienRect)) // �ӵ��������˷�����ײ
			{
				// ���·���
				curScore += alien->getScore();
				// �����Ч
				if (doPlayEffect)
					effectAudio->playEffect("bulletEffect.mp3");

				char scoreStr[25];
				sprintf(scoreStr, "current score :  %d", curScore);
				if (curScore > highScore) {
					highScore = curScore;
					char highStr[25];
					sprintf(highStr, "highest score :  %d", curScore);
					updateHighScore();
					highScoreLabel->setString(highStr);
				}
				scoreLabel->setString(scoreStr);
				
				alien->removeFromParentAndCleanup(true); // ��ɾ����������Ϊ����Ѫ��
				aliens.eraseObject(alien);
				
				bullet->removeFromParentAndCleanup(true); // ��ɾ�������������ӵ������ж�
				shipBullets.eraseObject(bullet);
							
				i--;
				break;
			}
		}
	}
}

void GameScene::checkCollision_ShipAndAlien(float t)
{
	Rect shipRect = ship->getBoundingBox();
	for (int i = 0; i < aliens.size(); i++)
	{
		auto alien = aliens.at(i);
		Rect alienRect = alien->getBoundingBox();
		if (shipRect.intersectsRect(alienRect))
		{
			ship->lives--;
			updateLivesLabel();
			explosionEffect();

			alien->removeFromParentAndCleanup(true);
			aliens.eraseObject(alien);			
			i--;

			if (ship->lives == 0) {
				gameOver();
			}
		}
	}
}

void GameScene::gameOver()
{
	char str[] = "GAME OVER!\npress 'Esc' to the menu, press 'U' to a new game";
	gameoverLabel = Label::create();
	Vec2 anchorPoint = gameoverLabel->getAnchorPoint();
	Vec2 position = Vec2(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);
	initLabelWithText(str, gameoverLabel, Color4B::ORANGE, 50, anchorPoint, position);
	this->addChild(gameoverLabel);

	enableRestart = true;
	// ֹͣ����ĵ��ȹ���
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateGameTime)); // �����Ϸ����ʱ��
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateShip)); // ���·ɴ��ƶ�
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlien)); // ���������˵�λ��
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateBullet)); // �����ӵ���λ��
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_ShipAndAlien)); // ���ɴ�����������ײ
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndAlien)); // ����ӵ�����������ײ
	this->unschedule("create_alien_first_key"); // ���ñ�ǩ��������
	this->unschedule("create_alien_second_key");// ֹͣ��������������
	this->unschedule("create_alien_third_key"); // ֹͣ��������������
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet)); // �����˷����ӵ�
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlienBullet)); // ���������˷�����ӵ�
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndShip)); // ����������Ƿ���зɴ�
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_one)); 
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_two));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_three));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndBoss));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createAddLivesImg));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkDoAddLives));
}

void GameScene::updateHighScore() // ������ʷ��߷�
{
	std::string highScoreStr = std::string(std::to_string(highScore)); // C++ 11
	std::string highScorePath = FileUtils::getInstance()->fullPathForFilename("highScore.txt");

	writeFile(highScorePath, highScoreStr);
}

void GameScene::startCreateAlienSecond(float)
{
	// ʹ��lambda���ʽֱ�ӵ��ȣ�C++ 11
	this->schedule([this](float dt) {
		this->createAlien("alien2.png");
		}, createAlienSecondInterval, "create_alien_second_key"); // ������������Ϊ�������ı�ǩ
}

void GameScene::startCreateAlienThird(float)
{
	this->isCreatingAlienThird = true;
	// ʹ��lambda���ʽֱ�ӵ��ȣ�C++ 11
	this->schedule([this](float dt) {
		this->createAlien("alien3.png");
		}, createAlienThirdInterval, "create_alien_third_key"); // ������������Ϊ�������ı�ǩ
}

void GameScene::updateGameTime(float t)
{
	gameTime += t;
	if (gameTime >= bossChapter)
	{
		// ����Boss��
		isBossChapter = true;
		auto curPos = boss->getPosition();
		bgmAudio->stopBackgroundMusic(); // ֹͣbgm1
		// Boss����
		if (curPos.y >= DISPLAY_HEIGHT / 6 * 5) {
			curPos.y -= boss->getSpeedY();
			boss->setPosition(curPos);
		}
		else // Boss��ʼ�����ɴ�
		{
			bgmAudio->playBackgroundMusic(bgmPath2, true); // ����bgm2��ע���ļ������ô����֣���Ƶ�ļ�����ʶ������·��
			// ֹͣ������ͨ������
			this->unschedule("create_alien_third_key"); // ������������Ϊ�������ı�ǩ
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet));
			this->unschedule("create_alien_second_key"); // ������������Ϊ�������ı�ǩ
			this->unschedule("create_alien_first_key"); // ��ʱ����һ��һ��������
			// ��ʼ����boss�ӵ�
			bulletOneInterval = std::max(1.0f, float(bulletOneInterval - 0.1));
			bulletTwoInterval = std::max(3.0f, float(bulletOneInterval - 0.3));
			bulletThreeInterval = std::max(1.5f, float(bulletOneInterval - 0.2));
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateGameTime));
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_one), bulletOneInterval);
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_two), bulletTwoInterval);
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_three), bulletThreeInterval);
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndBoss));
			showBossLives();
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::bossFleeing));
		}
	}
}

void GameScene::explosionEffect()
{
	auto shipPos = ship->getPosition();
	auto explosion = cocos2d::ParticleMeteor::create(); // ����ʹ��ParticleSystemQuad
	explosion->setPosition(shipPos);
	explosion->setSpeed(500); // ���ӵ��˶��ٶ�
	explosion->setEmissionRate(1000); // ÿ�뷢���������
	explosion->setDuration(0.1f); // ����ϵͳ����ը��Ч���ĳ���ʱ��
	explosion->setLife(0.05f); // �������ӵĳ���ʱ��
	// �������ӵ���ɫ
	explosion->setStartColor(Color4F(1.0f, 0.5f, 0.0f, 1.0f)); // ������ʼ��ɫΪ��ɫ
	explosion->setEndColor(Color4F(0.0f, 0.0f, 0.0f, 0.0f)); // ���ý�����ɫΪ͸��

	explosion->setAutoRemoveOnFinish(true); // ��Ч��ɺ��Զ��Ƴ�
	this->addChild(explosion);

	// �����Ч
	if(doPlayEffect)
		effectAudio->playEffect("shipCollisionEffect.mp3");// ����ʹ��Ĭ�ϲ���
}

void GameScene::updateLivesLabel()
{
	char str[30];
	sprintf(str, "remanent lives :  %d", ship->lives);
	livesLabel->setString(str);
}

void GameScene::checkCollision_BulletAndShip(float)
{
	for (int i = 0; i < alienBullets.size(); i++)
	{
		auto bullet = alienBullets.at(i);
		auto bulletRect = bullet->getBoundingBox();
		auto shipRect = ship->getBoundingBox();
		if (bulletRect.intersectsRect(shipRect))
		{
			ship->lives--;
			updateLivesLabel();
			explosionEffect();

			bullet->removeFromParentAndCleanup(true);
			alienBullets.eraseObject(bullet);
			--i;

			if (ship->lives == 0)
				gameOver();
		}
	}
}

void GameScene::createBossBullet_one(float)
{
	auto bullet = boss->createBossBullet_one("yellowBullet.png");
	bullet->label = 1; // �޸ı�ǩ
	bullet->targetPos = ship->getPosition();
	alienBullets.pushBack(bullet);
	this->addChild(bullet);
}

void GameScene::createBossBullet_two(float)
{
	float k = DISPLAY_WIDTH / 10;
	for (float i = 0; i < DISPLAY_WIDTH; i += k)
	{
		auto bullet = boss->createBossBullet_two(i);
		bullet->label = 2; // �޸ı�ǩ
		alienBullets.pushBack(bullet);
		this->addChild(bullet);
	}
}

void GameScene::createBossBullet_three(float)
{
	for (int i = -3; i <= 3; i++)
	{
		auto bullet = boss->createBossBullet_one("greenBullet.png");
		bullet->label = 3; // �޸ı�ǩ
		float centerX = DISPLAY_WIDTH / 2;
		float interval = DISPLAY_WIDTH / 3;
		bullet->targetPos.x = centerX - i * interval;
		bullet->targetPos.y = 0;
		alienBullets.pushBack(bullet);
		this->addChild(bullet);
	}
}

void GameScene::checkCollision_BulletAndBoss(float)
{
	for (int i = 0; i < shipBullets.size(); i++)
	{
		auto bullet = shipBullets.at(i);
		auto bulletRect = bullet->getBoundingBox();
		auto bossRect = boss->getBoundingBox();

		if (bulletRect.intersectsRect(bossRect))
		{
			boss->lives--;
			updateBossLivesImg();
			// �����Ч
			if (doPlayEffect)
				effectAudio->playEffect("bulletEffect.mp3");

			if (boss->lives == 0) // ���·���
			{
				doBossFleeing = true;
				isBossChapter = false;
				// ���·���
				curScore += boss->getScore();
				char scoreStr[25];
				sprintf(scoreStr, "current score :  %d", curScore);
				if (curScore > highScore) {
					highScore = curScore;
					char highStr[25];
					sprintf(highStr, "highest score :  %d", curScore);
					updateHighScore();
					highScoreLabel->setString(highStr);
				}
				scoreLabel->setString(scoreStr);
			}
			bullet->removeFromParentAndCleanup(true); // ��ɾ�������������ӵ������ж�
			shipBullets.eraseObject(bullet);
			i--;
			break;
		}
	}
}

void GameScene::bossFleeing(float)
{
	if(doBossFleeing)
	{
		bgmAudio->stopBackgroundMusic(); // ֹͣ��ǰbgm2
		auto curPos = boss->getPosition();
		auto bossSize = boss->getContentSize();
		if (curPos.y < DISPLAY_HEIGHT + bossSize.height + 1) {
			curPos.y += boss->getSpeedY();
			boss->setPosition(curPos);
		}
		else {
			bgmAudio->playBackgroundMusic(bgmPath1, true); // ��ʼ����bgm1
			// ֹͣ����boss�ӵ���boss���ڴ���״̬
			doBossFleeing = false;
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_one));
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_two));
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_three));
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndBoss));
			//this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::showBossLives));
			boss->lives = bossTotalLives;
			// ׼����ʼ��һ�ִ���Ϸ����Ϸ�ѶȼӴ�
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateGameTime));
			this->gameTime = 0.0f;
			// �����Ѷ�ǰ������ֹͣ��ǰ����

			// �ı䴴�����
			this->createAlienFirstInterval = std::max(1.0f, float(createAlienFirstInterval - difficulty));
			this->createAlienSecondInterval = std::max(1.0f, float(createAlienSecondInterval - difficulty));
			this->createAlienThirdInterval = std::max(1.0f, float(createAlienThirdInterval - difficulty));
			this->alienThirdShootInterval = std::max(2.0f, float(alienThirdShootInterval - bulletSpeedUpDiff));
			// ���µ���
			this->schedule([this](float dt) {
				this->createAlien("alien3.png");
				}, createAlienThirdInterval, "create_alien_third_key"); // ������������Ϊ�������ı�ǩ
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet), alienThirdShootInterval);
			this->schedule([this](float dt) {
				this->createAlien("alien2.png");
				}, createAlienSecondInterval, "create_alien_second_key"); // ������������Ϊ�������ı�ǩ
			this->schedule([this](float) {
				this->createAlien("alien1.png");
				}, createAlienFirstInterval, "create_alien_first_key"); // ��ʱ����һ��һ��������
		}
	}
}

void GameScene::createAddLivesImg(float)
{
	auto img = Sprite::create("lives.png");
	int x = rand() % 1500 + 20;
	int y = rand() % 800 + 20;
	img->setPosition(float(x), float(y));
	lives.pushBack(img);
	this->addChild(img);
}

void GameScene::checkDoAddLives(float)
{
	for (int i = 0; i < lives.size(); i++)
	{
		auto life = lives.at(i);
		auto lifeRec = life->getBoundingBox();
		auto shipRec = ship->getBoundingBox();
		if (lifeRec.intersectsRect(shipRec))
		{
			ship->lives++;
			this->updateLivesLabel();
			life->removeFromParentAndCleanup(true);
			lives.eraseObject(life);
			i--;
		}
	}
}

void GameScene::showBossLives()
{
	bossLivesImg = cocos2d::DrawNode::create();
	this->addChild(bossLivesImg);
	float left = 1200 - float(boss->lives) / bossTotalLives * 800;

	Vec2 rectVertices[] = {
		Vec2(left,870),   // ���½�
		Vec2(1200,870),  // ���½�
		Vec2(1200,890),  // ���Ͻ�
		Vec2(left,890),   // ���Ͻ�
	};

	Color4F rectColor(1.0f, 0.0f, 0.0f, 1.0f);
	bossLivesImg->drawPolygon(rectVertices, 4, rectColor, 0, rectColor);
}

void GameScene::updateBossLivesImg()
{
	float left = 1200 - float(boss->lives) / bossTotalLives * 800;
	if (boss->lives <= 0)
		left = 1200;
	bossLivesImg->clear(); // ���֮ǰ�ľ���
	Vec2 rectVertices[] = {
		Vec2(left,870),   // ���½�
		Vec2(1200,870),  // ���½�
		Vec2(1200,890),  // ���Ͻ�
		Vec2(left,890),   // ���Ͻ�
	};
	Color4F rectColor(1.0f, 0.0f, 0.0f, 1.0f); // ��ɫ
	bossLivesImg->drawPolygon(rectVertices, 4, rectColor, 0, rectColor);
}

void GameScene::playBGM(Ref* psender)
{
	doPlayBGM = doPlayBGM ? 0 : 1;
	if (doPlayBGM) {
		bgmItem->setNormalImage(Sprite::create("playMusic.png"));
		bgmItem->setSelectedImage(Sprite::create("stopMusic.png"));
		bgmAudio->resumeBackgroundMusic();
	}
	else {
		bgmItem->setNormalImage(Sprite::create("stopMusic.png"));
		bgmItem->setSelectedImage(Sprite::create("playMusic.png"));
		bgmAudio->pauseBackgroundMusic();
	}
}

void GameScene::playEffect(Ref* psender)
{
	doPlayEffect = doPlayEffect ? 0 : 1;
	if (doPlayEffect) {
		effectItem->setNormalImage(Sprite::create("playEffect.png"));
		effectItem->setSelectedImage(Sprite::create("stopEffect.png"));
	}
	else {
		effectItem->setNormalImage(Sprite::create("stopEffect.png"));
		effectItem->setSelectedImage(Sprite::create("playEffect.png"));
	}
}

void GameScene::saveData()
{
	rapidjson::Document doc;
	doc.SetObject();
	// �洢�����������Ϣ
	saveShipData(doc);
	saveAliensData(doc);
	saveShipBulletsData(doc);
	saveAlienBulletsData(doc);
	saveBossData(doc);
	saveNewLivesData(doc);
	// �洢��Ϸ���ȵ���Ϣ
	saveGameProgressData(doc);

	// ����Ϣд���ļ�
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string json = buffer.GetString();
	std::string filePath = FileUtils::getInstance()->fullPathForFilename("gameData.json");
	std::ofstream file(filePath);
	if (!file.is_open()) {
		CCLOG("ERROR : cannot open the file for writing");
		return;
	}
	file << json;
	file.close();
}

void GameScene::saveShipData(rapidjson::Document& doc)
{
	// �洢�ɴ�����Ϣ
	rapidjson::Value shipJs;
	shipJs.SetArray();
	// �洢�ɴ�������ֵ��Ϣ
	shipJs.PushBack(ship->lives, doc.GetAllocator());
	// �洢�ɴ���λ����Ϣ
	auto shipPos = ship->getPosition();
	float shipPosX = shipPos.x;
	float shipPosY = shipPos.y;
	shipJs.PushBack(shipPosX, doc.GetAllocator());
	shipJs.PushBack(shipPosY, doc.GetAllocator());
	doc.AddMember("shipMess", shipJs, doc.GetAllocator());
}
void GameScene::saveAliensData(rapidjson::Document& doc)
{
	// �洢�����˵���Ϣ
	rapidjson::Value alienJs;
	alienJs.SetArray();
	for (auto alien : aliens) // C++11
	{
		// ����һ�����飬�洢���������˵���Ϣ
		rapidjson::Value innerArray;
		innerArray.SetArray();
		// �ȴ��ǩ
		innerArray.PushBack(alien->label, doc.GetAllocator());
		// �ٴ�λ��
		auto alienPos = alien->getPosition();
		float alienPosX = alienPos.x;
		float alienPosY = alienPos.y;
		innerArray.PushBack(alienPosX, doc.GetAllocator());
		innerArray.PushBack(alienPosY, doc.GetAllocator());
		// dirx
		innerArray.PushBack(alien->dirX, doc.GetAllocator());
		// ��һ�������˵���������浽����������
		alienJs.PushBack(innerArray, doc.GetAllocator());
	}
	doc.AddMember("alienMess", alienJs, doc.GetAllocator());
}
void GameScene::saveShipBulletsData(rapidjson::Document& doc)
{
	// �洢�ɴ��ӵ�����Ϣ
	rapidjson::Value bulletsJs;
	bulletsJs.SetArray();
	for (auto bullet : shipBullets)
	{
		rapidjson::Value innerArray;
		innerArray.SetArray();
		// ֻ��λ��
		Vec2 pos = bullet->getPosition();
		float x = pos.x;
		float y = pos.y;
		innerArray.PushBack(x, doc.GetAllocator());
		innerArray.PushBack(y, doc.GetAllocator());
		bulletsJs.PushBack(innerArray, doc.GetAllocator());
	}
	doc.AddMember("shipBulletsMess", bulletsJs, doc.GetAllocator());
}
void GameScene::saveAlienBulletsData(rapidjson::Document& doc)
{
	// �洢�������ӵ�����Ϣ
	rapidjson::Value bulletsJs;
	bulletsJs.SetArray();
	for (auto bullet : alienBullets)
	{
		rapidjson::Value inner;
		inner.SetObject();
		// ��ǩ
		inner.AddMember("label", bullet->label, doc.GetAllocator());
		// λ��
		rapidjson::Value posArr;
		posArr.SetArray(); // û����仰���г����������ڷ���ռ����ָ������
		Vec2 pos = bullet->getPosition();
		float x = pos.x;
		float y = pos.y;
		posArr.PushBack(x, doc.GetAllocator());
		posArr.PushBack(y, doc.GetAllocator());
		inner.AddMember("bulletPos", posArr, doc.GetAllocator());
		// �Ƿ���boss_bullet_one
		inner.AddMember("isBossBulletOne", bullet->isBossBulletOne, doc.GetAllocator());
		// �ӵ��յ�
		rapidjson::Value dest;
		dest.SetArray();
		dest.PushBack(bullet->targetPos.x, doc.GetAllocator());
		dest.PushBack(bullet->targetPos.y, doc.GetAllocator());
		inner.AddMember("targetPos", dest, doc.GetAllocator());
		bulletsJs.PushBack(inner, doc.GetAllocator());
	}
	doc.AddMember("alienBulletsMess", bulletsJs, doc.GetAllocator());
}
void GameScene::saveBossData(rapidjson::Document& doc)
{
	rapidjson::Value bossJs;
	bossJs.SetArray();
	// boss������ֵ
	bossJs.PushBack(boss->lives, doc.GetAllocator());
	// boss��λ��
	auto bossPos = boss->getPosition();
	float x = bossPos.x;
	float y = bossPos.y;
	bossJs.PushBack(x, doc.GetAllocator());
	bossJs.PushBack(y, doc.GetAllocator());
	doc.AddMember("bossMess", bossJs, doc.GetAllocator());
}
void GameScene::saveGameProgressData(rapidjson::Document& doc)
{
	rapidjson::Value proJs;
	proJs.SetArray();
	// gameTime
	proJs.PushBack(gameTime, doc.GetAllocator());
	// current score
	proJs.PushBack(curScore, doc.GetAllocator());
	// interval
	rapidjson::Value intervalJs;
	intervalJs.SetObject();
	intervalJs.AddMember("alienFirstInterval", createAlienFirstInterval, doc.GetAllocator());
	intervalJs.AddMember("alienSecondInterval", createAlienSecondInterval, doc.GetAllocator());
	intervalJs.AddMember("alienThirdInterval", createAlienThirdInterval, doc.GetAllocator());
	intervalJs.AddMember("alienThirdShootInterval", alienThirdShootInterval, doc.GetAllocator());
	intervalJs.AddMember("bossBulletOneInterval", bulletOneInterval, doc.GetAllocator());
	intervalJs.AddMember("bossBulletTwoInterval", bulletTwoInterval, doc.GetAllocator());
	intervalJs.AddMember("bossBulletThreeInterval", bulletThreeInterval, doc.GetAllocator());

	proJs.PushBack(intervalJs, doc.GetAllocator());
	// isBossChapter
	proJs.PushBack(isBossChapter, doc.GetAllocator());

	doc.AddMember("gameProgress", proJs, doc.GetAllocator());
}
void GameScene::saveNewLivesData(rapidjson::Document& doc)
{
	rapidjson::Value livesJs;
	livesJs.SetArray();
	for (auto life : lives)
	{
		rapidjson::Value innerArray;
		innerArray.SetArray();
		Vec2 pos = life->getPosition();
		innerArray.PushBack(pos.x, doc.GetAllocator());
		innerArray.PushBack(pos.y, doc.GetAllocator());
		livesJs.PushBack(innerArray, doc.GetAllocator());
	}
	doc.AddMember("lives", livesJs, doc.GetAllocator());
}

bool GameScene::loadData()
{
	// ���ļ�
	std::string filePath = FileUtils::getInstance()->fullPathForFilename("gameData.json");
	std::ifstream file(filePath);
	if (!file.is_open()) {
		CCLOG("ERROR : cannot open the file for reading");
		return 0;
	}
	// ��json�ļ�д���ַ���
	std::string jsonData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	// ��ȡ����
	if (!jsonData.empty())
	{
		rapidjson::Document doc;
		doc.Parse(jsonData.c_str());

		bool isLiving = loadShip(doc);
		if (isLiving)
		{
			loadAlien(doc);
			loadShipBullet(doc);
			loadAlienBullet(doc);
			loadBoss(doc);
			loadGameProgress(doc);
			loadNewLives(doc);
		}

		return isLiving;
	}
	else
		return 0;
}

bool GameScene::loadShip(rapidjson::Document& doc)
{
	if (doc.HasMember("shipMess")) {
		const rapidjson::Value& ship_mess = doc["shipMess"];
		if (ship_mess.Size() == 3) {
			int lives = ship_mess[0].GetInt();
			ship->lives = lives;
			float x = ship_mess[1].GetDouble();
			float y = ship_mess[2].GetDouble();
			ship->setPosition(Vec2(x, y));
		}
	}
	else
		CCLOG("ERROR : cannot find ship_mess");
	if (ship->lives > 0)
		return true;
	return 0;
}
void GameScene::loadAlien(rapidjson::Document& doc)
{
	// set aliens
	if (doc.HasMember("alienMess")) {
		const rapidjson::Value& alien_message = doc["alienMess"];
		for (size_t i = 0; i < alien_message.Size(); i++) {
			Alien* alien;
			switch (alien_message[i][0].GetInt())
			{
				case 1:
					alien = Alien::create("alien1.png");
					break;
				case 2:
					alien = Alien_second::create("alien2.png");
					break;
				case 3:
					alien = Alien_third::create("alien3.png");
					break;
				default:
					;
			}
			float x = alien_message[i][1].GetDouble();
			float y = alien_message[i][2].GetDouble();
			alien->setPosition(Vec2(x, y));
			alien->dirX = alien_message[i][3].GetInt();
			aliens.pushBack(alien);
			this->addChild(alien);
		}
	}
}
void GameScene::loadShipBullet(rapidjson::Document& doc)
{
	if (doc.HasMember("shipBulletsMess")) {
		const rapidjson::Value& ship_bullets_mess = doc["shipBulletsMess"];
		for (size_t i = 0; i < ship_bullets_mess.Size(); i++)
		{
			Bullet* bullet = Bullet::create("whiteBullet.png");
			float x = ship_bullets_mess[i][0].GetDouble();
			float y = ship_bullets_mess[i][1].GetDouble();
			bullet->setPosition(Vec2(x, y));
			shipBullets.pushBack(bullet);
			this->addChild(bullet);
		}
	}
	else
		CCLOG("donot has shipBulletsMess");
}
void GameScene::loadAlienBullet(rapidjson::Document& doc)
{
	if (doc.HasMember("alienBulletsMess")) {
		const rapidjson::Value& alien_bullets_mess = doc["alienBulletsMess"];
		for (size_t i = 0; i < alien_bullets_mess.Size(); i++)
		{
			Bullet* bullet;
			if (alien_bullets_mess[i].HasMember("label")) {
				switch (alien_bullets_mess[i]["label"].GetInt())
				{
					case 0:
						bullet = Bullet::create("blueBullet.png");
						break;
					case 1:
						bullet = bossBullet_one::create("yellowBullet.png");
						break;
					case 2:
						bullet = Bullet::create("redBullet.png");
						break;
					case 3:
						bullet = bossBullet_one::create("greenBullet.png");
						break;
					default:
						;
				}
			}
			if (alien_bullets_mess[i].HasMember("bulletPos")) {
				float x = alien_bullets_mess[i]["bulletPos"][0].GetDouble();
				float y = alien_bullets_mess[i]["bulletPos"][1].GetDouble();
				bullet->setPosition(Vec2(x, y));
			}
			if (alien_bullets_mess[i].HasMember("isBossBulletOne")) {
				bullet->isBossBulletOne = alien_bullets_mess[i]["isBossBulletOne"].GetBool();
			}
			if (alien_bullets_mess[i].HasMember("targetPos")) {
				if (bullet->isBossBulletOne) {
					float x = alien_bullets_mess[i]["targetPos"][0].GetDouble();
					float y = alien_bullets_mess[i]["targetPos"][1].GetDouble();
					bullet->targetPos = Vec2(x, y);
				}
			}
			alienBullets.pushBack(bullet);
			this->addChild(bullet);
		}
	}
}
void GameScene::loadBoss(rapidjson::Document& doc)
{
	if (doc.HasMember("bossMess")) {
		const rapidjson::Value& boss_mess = doc["bossMess"];
		if (boss_mess.Size() == 3) {
			int lives = boss_mess[0].GetInt();
			boss->lives = lives;
			float x = boss_mess[1].GetDouble();
			float y = boss_mess[2].GetDouble();
			boss->setPosition(Vec2(x, y));
		}
	}
}
void GameScene::loadGameProgress(rapidjson::Document& doc)
{
	if (doc.HasMember("gameProgress")) {
		const rapidjson::Value& progress_mess = doc["gameProgress"];
		gameTime = progress_mess[0].GetDouble();
		curScore = progress_mess[1].GetInt();
		createAlienFirstInterval = progress_mess[2]["alienFirstInterval"].GetDouble();
		createAlienSecondInterval = progress_mess[2]["alienSecondInterval"].GetDouble();
		createAlienThirdInterval = progress_mess[2]["alienThirdInterval"].GetDouble();
		alienThirdShootInterval = progress_mess[2]["alienThirdShootInterval"].GetDouble();
		bulletOneInterval = progress_mess[2]["bossBulletOneInterval"].GetDouble();
		bulletTwoInterval = progress_mess[2]["bossBulletTwoInterval"].GetDouble();
		bulletThreeInterval = progress_mess[2]["bossBulletThreeInterval"].GetDouble();
		isBossChapter = progress_mess[3].GetBool();
	}
}
void GameScene::loadNewLives(rapidjson::Document& doc)
{
	if (doc.HasMember("lives")) {
		Sprite* life;
		const rapidjson::Value& lives_mess = doc["lives"];
		for (int i = 0; i < lives_mess.Size(); i++)
		{
			float x = lives_mess[i][0].GetDouble();
			float y = lives_mess[i][1].GetDouble();
			life = Sprite::create("lives.png");
			life->setPosition(Vec2(x, y));
			lives.pushBack(life);
			this->addChild(life);
		}
	}
}