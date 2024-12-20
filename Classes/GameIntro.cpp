#include <fstream>
#include <string>
#include "GameIntro.h"
#include "GameScene.h"
#include "TwoPlayerScene.h"
#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")  // utf-8编码使汉字正常显示

USING_NS_CC;

Scene* GameIntroScene::createScene()
{
	return GameIntroScene::create();
}

bool GameIntroScene::init()
{
	if (!Scene::init()) {
		return false;
	}

	setupUI();
	return true;
}

void GameIntroScene::setupUI()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto width = visibleSize.width;
	auto height = visibleSize.height;
	CCLOG("%.2f %.2f", width, height);
	// 背景
	auto background = LayerColor::create(Color4B(55, 55, 55, 255));
	this->addChild(background);
	// 标题
	auto titleLabel = Label::createWithSystemFont("游戏介绍", "Arial", 45); // 第二个参数使汉字正常显示
	titleLabel->setPosition(width / 2, height - 50);
	this->addChild(titleLabel);
	// 介绍文字
	std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename("introduction.txt");
	std::ifstream file(fullPath);
	if (!file.is_open()) {
		CCLOG("ERROR : fail to open the file");
		return;
	}
	std::string txtStr;
	std::string temp;
	while (std::getline(file, temp))
		txtStr = txtStr + temp + '\n';
	file.close();
	auto infoLabel = Label::createWithSystemFont(txtStr, "Arial", 24);
	infoLabel->setPosition(width / 2, height / 2);
	this->addChild(infoLabel);
	// 返回菜单
	auto backButton = MenuItemLabel::create(Label::createWithSystemFont("返回菜单", "Arial", 36),
		[](Ref* sender){
			auto scene = HelloWorld::createScene();
			Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
			});
	backButton->setPosition(width / 4, 50);
	// 开始游戏
	auto playButton = MenuItemLabel::create(Label::createWithSystemFont("开始游戏", "Arial", 36),
		[](Ref* sender) {
			auto scene = GameScene::createGameScene();
			Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
		});
	playButton->setPosition(width / 4 * 2, 50);
	// 双人模式
	auto twoPlayerButton = MenuItemLabel::create(Label::createWithSystemFont("双人模式", "Arial", 36),
		[](Ref* sender) {
			auto scene = twoPlayerScene::createGameScene();
			Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
		});
	twoPlayerButton->setPosition(width / 4 * 3, 50);
	auto menu = Menu::create(backButton, playButton, twoPlayerButton, nullptr);
	menu->setAnchorPoint(Vec2::ZERO);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
}