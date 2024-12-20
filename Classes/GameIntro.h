#pragma once
#ifndef __GAME_INTRO_H__
#define __GAME_INTRO_H__

#include "cocos2d.h"

class GameIntroScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameIntroScene);

private:
	void setupUI();
};

#endif