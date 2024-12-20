#pragma once
#ifndef __COMMON_FUNC__
#define __COMMON_FUNC__
#include <fstream>
#include "cocos2d.h"
USING_NS_CC;

void initLabelWithText(std::string str, Label* label, Color4B color,
	int fontSize, Vec2 anchorPoint, Vec2 position);

std::string readFile(const std::string& filename);

void writeFile(const std::string& filename, const std::string& content);


#endif // !__COMMON_FUNC__