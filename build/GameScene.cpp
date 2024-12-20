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

	//设置背景音乐并不断播放它
	bgmAudio->playBackgroundMusic(bgmPath1, true);
	// 是否播放背景音乐的指示牌
	bgmItem = MenuItemImage::create(
		"playMusic.png", // 正常状态下显示的图片
		"stopMusic.png", // 选中状态下显示的图片
		CC_CALLBACK_1(GameScene::playBGM, this)
	);
	bgmItem->setAnchorPoint(Vec2(1, 1));
	bgmItem->setPosition(Vec2(windowSize.width - 50, windowSize.height - 100));
	// 是否播放音效的提示牌
	effectItem = MenuItemImage::create(
		"playEffect.png", // 正常状态下显示的图片
		"stopEffect.png", // 选中状态下显示的图片
		CC_CALLBACK_1(GameScene::playEffect, this)
	);
	effectItem->setAnchorPoint(Vec2(1, 1));
	effectItem->setPosition(Vec2(windowSize.width - 50, windowSize.height - 165));
	// Menu
	auto itemMenu = Menu::create(bgmItem, effectItem, nullptr);
	itemMenu->setPosition(Vec2::ZERO);
	this->addChild(itemMenu);
	// 创建飞船
	ship = Ship::create("ship1.png");
	// 创建Boss
	boss = Alien_boss::create("Boss.png");
	// 根据读档结果设置飞船坐标和boss坐标
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
	// 分数牌——当前分数
	scoreLabel = Label::create();
	std::string curStr = "current score : " + std::to_string(curScore);
	initLabelWithText(curStr, scoreLabel, Color4B(255, 0, 0, 122),
		30, Vec2(1, 1), Vec2(windowSize.width - 50, windowSize.height - 50));
	this->addChild(scoreLabel);
	// 分数牌——历史最高分数
	std::string highScorePath = FileUtils::getInstance()->fullPathForFilename("highScore.txt");
	std::string highStr = readFile(highScorePath);
	highScore = stoi(highStr);
	highScoreLabel = Label::create();
	highStr = "highest score :  " + std::to_string(highScore);
	initLabelWithText(highStr, highScoreLabel, Color4B(255, 0, 0, 122),
		30, Vec2(0, 1), Vec2(50, windowSize.height - 50));
	this->addChild(highScoreLabel);
	// 提示牌——剩余生命值
	livesLabel = Label::create();
	char str[30];
	sprintf(str, "remanet lives :  %d", ship->lives);
	initLabelWithText(str, livesLabel, Color4B(0, 0, 255, 200),
		30, Vec2(0, 0), Vec2(50, 50));
	this->addChild(livesLabel);

	///////////下面是对游戏元素的运动处理
	// 移动飞船
	auto listener = EventListenerKeyboard::create();
	// 设置按下键和释放键的回调
	listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	// 将监听器添加到事件分发器
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	// 定时器
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateShip)); // 更新飞船移动
	//Director::getInstance()->
	if(!isBossChapter)
	{
		this->schedule([this](float) {
			this->createAlien("alien1.png");
			}, createAlienFirstInterval, "create_alien_first_key"); // 定时创建一个一号外星人
		float t1 = gameTime >= 6.0f ? 0.0f : 6.0f - gameTime;
		float t2 = gameTime >= 8.0f ? 0.0f : 8.0f - gameTime;
		this->scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::startCreateAlienSecond), t1);// 6秒后开始创建二号外星人
		this->scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::startCreateAlienThird), t2); // 8秒后开始创建三号外星人
	}
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlien)); // 更新外星人的位置
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateBullet)); // 更新飞船子弹的位置
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet), alienThirdShootInterval); // 创建三号外星人子弹
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlienBullet)); // 更新外星人子弹的位置
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndShip)); // 检查外星人是否击中飞船
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_ShipAndAlien)); // 检查飞船与外星人相撞
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndAlien)); // 检查子弹与外星人相撞
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateGameTime)); // 增加游戏难度
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createAddLivesImg), 60.0f); // 补充生命值
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
	auto scene = HelloWorld::createScene(); // 回溯菜单
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
				this->scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::createShipBullet), 0.0f); // 飞船发射子弹
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
		// 产生随机的左右移动方向
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
		// 更新左右移动
		if (alienPos.x < alienSize.width / 2 + MOVE_BIAS ||
			alienPos.x > DISPLAY_WIDTH - alienSize.width / 2 - MOVE_BIAS)
			alien->dirX *= -1; // 碰壁反方向
		alienPos.x += alien->alienSpeedX * alien->dirX;
		// 持续向下移动
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
			break; // 安全性判断
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
			break; // 安全性判断
		auto bulletPos = bullet->getPosition();
		auto bulletSize = bullet->getContentSize();
		// 左右不移动
		// 一直向上移动
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
		for (int j = 0; j < aliens.size(); j++) // 检查子弹与外星人的碰撞
		{
			auto alien = aliens.at(j);
			auto alienRect = alien->getBoundingBox();
			if (bulletRect.intersectsRect(alienRect)) // 子弹与外星人发生碰撞
			{
				// 更新分数
				curScore += alien->getScore();
				// 添加音效
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
				
				alien->removeFromParentAndCleanup(true); // 先删除，后续改为减少血量
				aliens.eraseObject(alien);
				
				bullet->removeFromParentAndCleanup(true); // 先删除，后续依据子弹类型判断
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
	// 停止下面的调度工作
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateGameTime)); // 获得游戏持续时间
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateShip)); // 更新飞船移动
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlien)); // 更新外星人的位置
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateBullet)); // 更新子弹的位置
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_ShipAndAlien)); // 检查飞船与外星人相撞
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndAlien)); // 检查子弹与外星人相撞
	this->unschedule("create_alien_first_key"); // 利用标签结束调度
	this->unschedule("create_alien_second_key");// 停止创建二号外星人
	this->unschedule("create_alien_third_key"); // 停止创建三号外星人
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet)); // 外星人发射子弹
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::updateAlienBullet)); // 更新外星人发射的子弹
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndShip)); // 检查外星人是否击中飞船
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_one)); 
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_two));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_three));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndBoss));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createAddLivesImg));
	this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkDoAddLives));
}

void GameScene::updateHighScore() // 更新历史最高分
{
	std::string highScoreStr = std::string(std::to_string(highScore)); // C++ 11
	std::string highScorePath = FileUtils::getInstance()->fullPathForFilename("highScore.txt");

	writeFile(highScorePath, highScoreStr);
}

void GameScene::startCreateAlienSecond(float)
{
	// 使用lambda表达式直接调度，C++ 11
	this->schedule([this](float dt) {
		this->createAlien("alien2.png");
		}, createAlienSecondInterval, "create_alien_second_key"); // 第三个参数作为这个任务的标签
}

void GameScene::startCreateAlienThird(float)
{
	this->isCreatingAlienThird = true;
	// 使用lambda表达式直接调度，C++ 11
	this->schedule([this](float dt) {
		this->createAlien("alien3.png");
		}, createAlienThirdInterval, "create_alien_third_key"); // 第三个参数作为这个任务的标签
}

void GameScene::updateGameTime(float t)
{
	gameTime += t;
	if (gameTime >= bossChapter)
	{
		// 进入Boss关
		isBossChapter = true;
		auto curPos = boss->getPosition();
		bgmAudio->stopBackgroundMusic(); // 停止bgm1
		// Boss进场
		if (curPos.y >= DISPLAY_HEIGHT / 6 * 5) {
			curPos.y -= boss->getSpeedY();
			boss->setPosition(curPos);
		}
		else // Boss开始攻击飞船
		{
			bgmAudio->playBackgroundMusic(bgmPath2, true); // 播放bgm2，注意文件名不得带数字，音频文件不能识别数字路径
			// 停止创建普通外星人
			this->unschedule("create_alien_third_key"); // 第三个参数作为这个任务的标签
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet));
			this->unschedule("create_alien_second_key"); // 第三个参数作为这个任务的标签
			this->unschedule("create_alien_first_key"); // 定时创建一个一号外星人
			// 开始创建boss子弹
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
	auto explosion = cocos2d::ParticleMeteor::create(); // 或者使用ParticleSystemQuad
	explosion->setPosition(shipPos);
	explosion->setSpeed(500); // 粒子的运动速度
	explosion->setEmissionRate(1000); // 每秒发射的粒子数
	explosion->setDuration(0.1f); // 粒子系统（爆炸特效）的持续时间
	explosion->setLife(0.05f); // 单个粒子的持续时间
	// 设置粒子的颜色
	explosion->setStartColor(Color4F(1.0f, 0.5f, 0.0f, 1.0f)); // 设置起始颜色为橙色
	explosion->setEndColor(Color4F(0.0f, 0.0f, 0.0f, 0.0f)); // 设置结束颜色为透明

	explosion->setAutoRemoveOnFinish(true); // 特效完成后自动移除
	this->addChild(explosion);

	// 添加音效
	if(doPlayEffect)
		effectAudio->playEffect("shipCollisionEffect.mp3");// 其他使用默认参数
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
	bullet->label = 1; // 修改标签
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
		bullet->label = 2; // 修改标签
		alienBullets.pushBack(bullet);
		this->addChild(bullet);
	}
}

void GameScene::createBossBullet_three(float)
{
	for (int i = -3; i <= 3; i++)
	{
		auto bullet = boss->createBossBullet_one("greenBullet.png");
		bullet->label = 3; // 修改标签
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
			// 添加音效
			if (doPlayEffect)
				effectAudio->playEffect("bulletEffect.mp3");

			if (boss->lives == 0) // 更新分数
			{
				doBossFleeing = true;
				isBossChapter = false;
				// 更新分数
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
			bullet->removeFromParentAndCleanup(true); // 先删除，后续依据子弹类型判断
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
		bgmAudio->stopBackgroundMusic(); // 停止当前bgm2
		auto curPos = boss->getPosition();
		auto bossSize = boss->getContentSize();
		if (curPos.y < DISPLAY_HEIGHT + bossSize.height + 1) {
			curPos.y += boss->getSpeedY();
			boss->setPosition(curPos);
		}
		else {
			bgmAudio->playBackgroundMusic(bgmPath1, true); // 开始播放bgm1
			// 停止创建boss子弹，boss处于待机状态
			doBossFleeing = false;
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_one));
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_two));
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::createBossBullet_three));
			this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollision_BulletAndBoss));
			//this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::showBossLives));
			boss->lives = bossTotalLives;
			// 准备开始新一轮次游戏，游戏难度加大
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::updateGameTime));
			this->gameTime = 0.0f;
			// 增加难度前，需先停止当前调度

			// 改变创建间隔
			this->createAlienFirstInterval = std::max(1.0f, float(createAlienFirstInterval - difficulty));
			this->createAlienSecondInterval = std::max(1.0f, float(createAlienSecondInterval - difficulty));
			this->createAlienThirdInterval = std::max(1.0f, float(createAlienThirdInterval - difficulty));
			this->alienThirdShootInterval = std::max(2.0f, float(alienThirdShootInterval - bulletSpeedUpDiff));
			// 重新调度
			this->schedule([this](float dt) {
				this->createAlien("alien3.png");
				}, createAlienThirdInterval, "create_alien_third_key"); // 第三个参数作为这个任务的标签
			this->schedule(CC_SCHEDULE_SELECTOR(GameScene::createAlienBullet), alienThirdShootInterval);
			this->schedule([this](float dt) {
				this->createAlien("alien2.png");
				}, createAlienSecondInterval, "create_alien_second_key"); // 第三个参数作为这个任务的标签
			this->schedule([this](float) {
				this->createAlien("alien1.png");
				}, createAlienFirstInterval, "create_alien_first_key"); // 定时创建一个一号外星人
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
		Vec2(left,870),   // 左下角
		Vec2(1200,870),  // 右下角
		Vec2(1200,890),  // 右上角
		Vec2(left,890),   // 左上角
	};

	Color4F rectColor(1.0f, 0.0f, 0.0f, 1.0f);
	bossLivesImg->drawPolygon(rectVertices, 4, rectColor, 0, rectColor);
}

void GameScene::updateBossLivesImg()
{
	float left = 1200 - float(boss->lives) / bossTotalLives * 800;
	if (boss->lives <= 0)
		left = 1200;
	bossLivesImg->clear(); // 清空之前的矩形
	Vec2 rectVertices[] = {
		Vec2(left,870),   // 左下角
		Vec2(1200,870),  // 右下角
		Vec2(1200,890),  // 右上角
		Vec2(left,890),   // 左上角
	};
	Color4F rectColor(1.0f, 0.0f, 0.0f, 1.0f); // 红色
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
	// 存储各个精灵的信息
	saveShipData(doc);
	saveAliensData(doc);
	saveShipBulletsData(doc);
	saveAlienBulletsData(doc);
	saveBossData(doc);
	saveNewLivesData(doc);
	// 存储游戏进度的信息
	saveGameProgressData(doc);

	// 将信息写入文件
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
	// 存储飞船的信息
	rapidjson::Value shipJs;
	shipJs.SetArray();
	// 存储飞船的生命值信息
	shipJs.PushBack(ship->lives, doc.GetAllocator());
	// 存储飞船的位置信息
	auto shipPos = ship->getPosition();
	float shipPosX = shipPos.x;
	float shipPosY = shipPos.y;
	shipJs.PushBack(shipPosX, doc.GetAllocator());
	shipJs.PushBack(shipPosY, doc.GetAllocator());
	doc.AddMember("shipMess", shipJs, doc.GetAllocator());
}
void GameScene::saveAliensData(rapidjson::Document& doc)
{
	// 存储外星人的信息
	rapidjson::Value alienJs;
	alienJs.SetArray();
	for (auto alien : aliens) // C++11
	{
		// 设置一个数组，存储单个外星人的信息
		rapidjson::Value innerArray;
		innerArray.SetArray();
		// 先存标签
		innerArray.PushBack(alien->label, doc.GetAllocator());
		// 再存位置
		auto alienPos = alien->getPosition();
		float alienPosX = alienPos.x;
		float alienPosY = alienPos.y;
		innerArray.PushBack(alienPosX, doc.GetAllocator());
		innerArray.PushBack(alienPosY, doc.GetAllocator());
		// dirx
		innerArray.PushBack(alien->dirX, doc.GetAllocator());
		// 将一个外星人的数据数组存到整体数组中
		alienJs.PushBack(innerArray, doc.GetAllocator());
	}
	doc.AddMember("alienMess", alienJs, doc.GetAllocator());
}
void GameScene::saveShipBulletsData(rapidjson::Document& doc)
{
	// 存储飞船子弹的信息
	rapidjson::Value bulletsJs;
	bulletsJs.SetArray();
	for (auto bullet : shipBullets)
	{
		rapidjson::Value innerArray;
		innerArray.SetArray();
		// 只存位置
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
	// 存储外星人子弹的信息
	rapidjson::Value bulletsJs;
	bulletsJs.SetArray();
	for (auto bullet : alienBullets)
	{
		rapidjson::Value inner;
		inner.SetObject();
		// 标签
		inner.AddMember("label", bullet->label, doc.GetAllocator());
		// 位置
		rapidjson::Value posArr;
		posArr.SetArray(); // 没有这句话运行出错，可能是在分配空间或者指定类型
		Vec2 pos = bullet->getPosition();
		float x = pos.x;
		float y = pos.y;
		posArr.PushBack(x, doc.GetAllocator());
		posArr.PushBack(y, doc.GetAllocator());
		inner.AddMember("bulletPos", posArr, doc.GetAllocator());
		// 是否是boss_bullet_one
		inner.AddMember("isBossBulletOne", bullet->isBossBulletOne, doc.GetAllocator());
		// 子弹终点
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
	// boss的生命值
	bossJs.PushBack(boss->lives, doc.GetAllocator());
	// boss的位置
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
	// 打开文件
	std::string filePath = FileUtils::getInstance()->fullPathForFilename("gameData.json");
	std::ifstream file(filePath);
	if (!file.is_open()) {
		CCLOG("ERROR : cannot open the file for reading");
		return 0;
	}
	// 将json文件写入字符串
	std::string jsonData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	// 读取数据
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