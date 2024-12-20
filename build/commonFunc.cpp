#include "commonFunc.h"

void initLabelWithText(std::string str, Label* label, Color4B color,
	int fontSize, Vec2 anchorPoint, Vec2 position)
{
	label->setString(str);
	label->setTextColor(color);
	label->setSystemFontSize(fontSize);
	label->setAnchorPoint(anchorPoint);
	label->setPosition(position);
}

std::string readFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		CCLOG("ERROR : cannot open the file");
		return "";
	}
	std::string str;
	while (std::getline(file, str))
		;
	file.close();

	return str;
}

void writeFile(const std::string& filename, const std::string& content)
{
	std::ofstream file(filename);
	if (!file.is_open()) {
		CCLOG("ERROR : cannot open the file");
		return;
	}
	file << content;
	file.flush();
	file.close();
}
