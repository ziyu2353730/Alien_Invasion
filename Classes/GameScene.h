#pragma once
#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include "cocos2d.h"
#include "Sprites.h"
#include "SimpleAudioEngine.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

using namespace CocosDenshion;

#pragma warning(disable: 4099) // 链接，由于cocos内部的原因
#pragma warning(disable: 4244) // int -> float 转换
#pragma warning(disable: 26495) // cocos 内部warning
#pragma warning(disable: 4018) // 有符号无符号不匹配

#define DISPLAY_WIDTH 1600
#define DISPLAY_HEIGHT 900
#define MOVE_BIAS 10

USING_NS_CC;

class GameScene :public Layer
{
public:
	// 创建场景
	static Scene* createGameScene();
	// 初始化
	virtual bool init();
	// 更新生命牌
	void updateLivesLabel();
	// 记录游戏持续时间
	void updateGameTime(float t);
	// 背景音乐控制
	void playBGM(Ref* psender);
	void playEffect(Ref* psender);
	// 移动飞船
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void updateShip(float t);
	// 创建子弹
	void createShipBullet(float time);
	// 移动子弹
	void updateBullet(float time);
	// 创建外星人
	void startCreateAlienSecond(float);
	void startCreateAlienThird(float);
	void createAlien(const char* filename);
	// 创建外星人的子弹
	void createAlienBullet(float);
	void updateAlienBullet(float);
	// 更新Boss的位置
	void bossFleeing(float);
	// 创建Boss子弹
	void createBossBullet_one(float);
	void createBossBullet_two(float);
	void createBossBullet_three(float);
	// 更新boss血条
	void showBossLives();
	void updateBossLivesImg();
	// 移动外星人
	void updateAlien(float time);
	// 检测碰撞
	void checkCollision_BulletAndAlien(float t);
	void checkCollision_ShipAndAlien(float t);
	void checkCollision_BulletAndShip(float);
	void checkCollision_BulletAndBoss(float);
	// 爆炸效果
	void explosionEffect();
	// 更新最高分
	void updateHighScore();
	// 游戏结束
	void gameOver();
	// 增加生命值
	void createAddLivesImg(float);
	void checkDoAddLives(float);
	// 存档
	void saveData();
	void saveShipData(rapidjson::Document& doc);
	void saveAliensData(rapidjson::Document& doc);
	void saveShipBulletsData(rapidjson::Document& doc);
	void saveAlienBulletsData(rapidjson::Document& doc);
	void saveBossData(rapidjson::Document& doc);
	void saveGameProgressData(rapidjson::Document& doc);
	void saveNewLivesData(rapidjson::Document& doc);
	// 载入数据
	bool loadData();
	bool loadShip(rapidjson::Document& doc);
	void loadAlien(rapidjson::Document& doc);
	void loadShipBullet(rapidjson::Document& doc);
	void loadAlienBullet(rapidjson::Document& doc);
	void loadBoss(rapidjson::Document& doc);
	void loadGameProgress(rapidjson::Document& doc);
	void loadNewLives(rapidjson::Document& doc);

	CREATE_FUNC(GameScene);

private:
	// 得分板
	Label* scoreLabel;
	Label* highScoreLabel;
	int curScore = 0;
	int highScore = 0;
	// 游戏提示板
	Label* gameoverLabel; // 游戏结束的提示语显示
	Label* livesLabel; // 玩家剩余生命值的提示
	MenuItemImage* bgmItem; // 是否播放音乐的提示
	MenuItemImage* effectItem; // 是否播放音效的提示
	// 游戏计时器
	float gameTime = 0;
	// 背景音乐播放
	SimpleAudioEngine* bgmAudio = SimpleAudioEngine::getInstance();	// 创建背景音乐
	bool doPlayBGM = true;
	std::string bgmonePath = FileUtils::getInstance()->fullPathForFilename("bgmone.mp3");
	std::string bgmtwoPath = FileUtils::getInstance()->fullPathForFilename("bgmtwo.mp3");
	const char* bgmPath1 = bgmonePath.c_str();
	const char* bgmPath2 = bgmtwoPath.c_str();
	// 音效播放
	SimpleAudioEngine* effectAudio = SimpleAudioEngine::getInstance();
	bool doPlayEffect = true;
	// 飞船
	Ship* ship;
	// 增加生命值
	Vector<Sprite*> lives;
	// 游戏难度递增系数
	float difficulty = 0.5f;
	float bulletSpeedUpDiff = 0.5f;
	//void increaseGameDifficulty(float t);
	// 子弹
	Vector<Bullet*> shipBullets;
	// 外星人
	float createAlienFirstInterval = 2.0f; // 创建一号外星人的时间间隔
	bool isCreatingAlienSecond = false; // 允许开始创建二号外星人
	float createAlienSecondInterval = 3.0f; // 创建二号外星人的时间间隔
	bool isCreatingAlienThird = false; // 允许开始创建三号外星人
	float createAlienThirdInterval = 6.0f; // 创建三号外星人的时间间隔
	float alienThirdShootInterval = 3.0f; // 三号外星人发射子弹的间隔
	Vector<Alien*> aliens;
	Vector<Bullet*> alienBullets; // 将外星人发射的子弹设置为GameScene的数据成员，
								  // 是为了当发射子弹的外星人被消灭后，它发射的子弹仍然能够被更新
	// Boss
	bool isBossChapter = false;
	Alien_boss* boss;
	float bossChapter = 100.0f;
	bool doBossFleeing = false;
	int bossTotalLives = 100;
	float bulletOneInterval = 1.5f;
	float bulletTwoInterval = 5.0f;
	float bulletThreeInterval = 2.5f;
	DrawNode* bossLivesImg; // boss血条
	// 重新开始游戏
	bool enableRestart = false;
};


#endif // !GAME_SCENE_