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

#pragma warning(disable: 4099) // ���ӣ�����cocos�ڲ���ԭ��
#pragma warning(disable: 4244) // int -> float ת��
#pragma warning(disable: 26495) // cocos �ڲ�warning
#pragma warning(disable: 4018) // �з����޷��Ų�ƥ��

#define DISPLAY_WIDTH 1600
#define DISPLAY_HEIGHT 900
#define MOVE_BIAS 10

USING_NS_CC;

class GameScene :public Layer
{
public:
	// ��������
	static Scene* createGameScene();
	// ��ʼ��
	virtual bool init();
	// ����������
	void updateLivesLabel();
	// ��¼��Ϸ����ʱ��
	void updateGameTime(float t);
	// �������ֿ���
	void playBGM(Ref* psender);
	void playEffect(Ref* psender);
	// �ƶ��ɴ�
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void updateShip(float t);
	// �����ӵ�
	void createShipBullet(float time);
	// �ƶ��ӵ�
	void updateBullet(float time);
	// ����������
	void startCreateAlienSecond(float);
	void startCreateAlienThird(float);
	void createAlien(const char* filename);
	// ���������˵��ӵ�
	void createAlienBullet(float);
	void updateAlienBullet(float);
	// ����Boss��λ��
	void bossFleeing(float);
	// ����Boss�ӵ�
	void createBossBullet_one(float);
	void createBossBullet_two(float);
	void createBossBullet_three(float);
	// ����bossѪ��
	void showBossLives();
	void updateBossLivesImg();
	// �ƶ�������
	void updateAlien(float time);
	// �����ײ
	void checkCollision_BulletAndAlien(float t);
	void checkCollision_ShipAndAlien(float t);
	void checkCollision_BulletAndShip(float);
	void checkCollision_BulletAndBoss(float);
	// ��ըЧ��
	void explosionEffect();
	// ������߷�
	void updateHighScore();
	// ��Ϸ����
	void gameOver();
	// ��������ֵ
	void createAddLivesImg(float);
	void checkDoAddLives(float);
	// �浵
	void saveData();
	void saveShipData(rapidjson::Document& doc);
	void saveAliensData(rapidjson::Document& doc);
	void saveShipBulletsData(rapidjson::Document& doc);
	void saveAlienBulletsData(rapidjson::Document& doc);
	void saveBossData(rapidjson::Document& doc);
	void saveGameProgressData(rapidjson::Document& doc);
	void saveNewLivesData(rapidjson::Document& doc);
	// ��������
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
	// �÷ְ�
	Label* scoreLabel;
	Label* highScoreLabel;
	int curScore = 0;
	int highScore = 0;
	// ��Ϸ��ʾ��
	Label* gameoverLabel; // ��Ϸ��������ʾ����ʾ
	Label* livesLabel; // ���ʣ������ֵ����ʾ
	MenuItemImage* bgmItem; // �Ƿ񲥷����ֵ���ʾ
	MenuItemImage* effectItem; // �Ƿ񲥷���Ч����ʾ
	// ��Ϸ��ʱ��
	float gameTime = 0;
	// �������ֲ���
	SimpleAudioEngine* bgmAudio = SimpleAudioEngine::getInstance();	// ������������
	bool doPlayBGM = true;
	std::string bgmonePath = FileUtils::getInstance()->fullPathForFilename("bgmone.mp3");
	std::string bgmtwoPath = FileUtils::getInstance()->fullPathForFilename("bgmtwo.mp3");
	const char* bgmPath1 = bgmonePath.c_str();
	const char* bgmPath2 = bgmtwoPath.c_str();
	// ��Ч����
	SimpleAudioEngine* effectAudio = SimpleAudioEngine::getInstance();
	bool doPlayEffect = true;
	// �ɴ�
	Ship* ship;
	// ��������ֵ
	Vector<Sprite*> lives;
	// ��Ϸ�Ѷȵ���ϵ��
	float difficulty = 0.5f;
	float bulletSpeedUpDiff = 0.5f;
	//void increaseGameDifficulty(float t);
	// �ӵ�
	Vector<Bullet*> shipBullets;
	// ������
	float createAlienFirstInterval = 2.0f; // ����һ�������˵�ʱ����
	bool isCreatingAlienSecond = false; // ����ʼ��������������
	float createAlienSecondInterval = 3.0f; // �������������˵�ʱ����
	bool isCreatingAlienThird = false; // ����ʼ��������������
	float createAlienThirdInterval = 6.0f; // �������������˵�ʱ����
	float alienThirdShootInterval = 3.0f; // ���������˷����ӵ��ļ��
	Vector<Alien*> aliens;
	Vector<Bullet*> alienBullets; // �������˷�����ӵ�����ΪGameScene�����ݳ�Ա��
								  // ��Ϊ�˵������ӵ��������˱��������������ӵ���Ȼ�ܹ�������
	// Boss
	bool isBossChapter = false;
	Alien_boss* boss;
	float bossChapter = 100.0f;
	bool doBossFleeing = false;
	int bossTotalLives = 100;
	float bulletOneInterval = 1.5f;
	float bulletTwoInterval = 5.0f;
	float bulletThreeInterval = 2.5f;
	DrawNode* bossLivesImg; // bossѪ��
	// ���¿�ʼ��Ϸ
	bool enableRestart = false;
};


#endif // !GAME_SCENE_