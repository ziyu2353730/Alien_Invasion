/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "../build/GameScene.h"
#include "../build/GameIntro.h"
#include "audio/include/AudioEngine.h"
#include "../build/TwoPlayerScene.h"

#define DISPLAY_WIDTH 1600
#define DISPLAY_HEIGHT 900

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
   /* if ( !Scene::init() )
    {
        return false;
    }*/
    if (!Scene::initWithPhysics())
    {
        return false;
    }
    Size windowSize = this->getContentSize();
    // 背景
    auto spriteBack = Sprite::create("background.jpg");
    spriteBack->setAnchorPoint(Vec2::ZERO);
    spriteBack->setPosition(Vec2::ZERO);
    this->addChild(spriteBack);
    // 音频预加载，提高进入游戏界面的丝滑度
    std::string bgmonePath = FileUtils::getInstance()->fullPathForFilename("bgmone.mp3");
    std::string bgmtwoPath = FileUtils::getInstance()->fullPathForFilename("bgmtwo.mp3");
    AudioEngine::preload(bgmonePath);
    AudioEngine::preload(bgmtwoPath);
    // 开始按钮
    auto startButton = MenuItemFont::create("GAME BEGIN", CC_CALLBACK_1(HelloWorld::startGame, this));
    startButton->setFontSizeObj(90);
    // 退出按钮
    auto endButton = MenuItemFont::create("GAME END", CC_CALLBACK_1(HelloWorld::endGame, this));
    endButton->setFontSizeObj(90);
    // 介绍按钮
    auto introduceButton = MenuItemFont::create("Introduction", CC_CALLBACK_1(HelloWorld::introduceGame, this));
    introduceButton->setFontSizeObj(90);
    // 双人模式
    auto twoPlayerButton = MenuItemFont::create("Two Player", CC_CALLBACK_1(HelloWorld::twoPlayer, this));
    twoPlayerButton->setFontSizeObj(90);
    // 设置按钮位置
    startButton->setPosition(Vec2(windowSize.width / 2, windowSize.height / 5 * 4));
    twoPlayerButton->setPosition(Vec2(windowSize.width / 2, windowSize.height / 5 * 3));
    introduceButton->setPosition(Vec2(windowSize.width / 2, windowSize.height / 5 * 2));
    endButton->setPosition(Vec2(windowSize.width / 2, windowSize.height / 5));   

    Vector<MenuItem*> MenuItems;
    MenuItems.pushBack(startButton);
    MenuItems.pushBack(introduceButton);
    MenuItems.pushBack(endButton);
    MenuItems.pushBack(twoPlayerButton);
    auto menu = Menu::createWithArray(MenuItems);
    menu->setAnchorPoint(Vec2::ZERO);
    menu->setPosition(Vec2::ZERO);
    
    this->addChild(menu, 1);
    return true;
}
void HelloWorld::startGame(Ref* psender)
{
    auto scene = GameScene::createGameScene();
    Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
}
void HelloWorld::twoPlayer(Ref* psender)
{
    auto scene = twoPlayerScene::createGameScene();
    Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
}
void HelloWorld::endGame(Ref* psender)
{
    Director::getInstance()->end();
}
void HelloWorld::introduceGame(Ref* psender)
{
    auto scene = GameIntroScene::createScene();
    Director::getInstance()->replaceScene(TransitionSlideInT::create(1, scene));
}
void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
