#include "InputBase.h"

InputBase::InputBase(void)
{
}

void InputBase::Update(std::pair<const int, InputBase::Info>& p)
{
	p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
	p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;

	// âΩÇ©Ç™ç°âüÇ≥ÇÍÇΩÇ©
	if (!isAnyoneDown_) {

		isAnyoneDown_ = p.second.keyTrgDown;
		anyoneKey_ = p.second.keyTrgDown;
		anyone_ = p.second.keyTrgDown;
	}
	// âΩÇ©Ç™âüÇ≥ÇÍÇƒÇ¢ÇÈÇ©
	if (!anyoneKey_) {

		anyoneKey_ = p.second.keyNew;
		anyone_ = p.second.keyNew;
	}
}

void InputBase::Add(int key, KEYPAD_NO type)
{
	InputBase::Info info = InputBase::Info();
	info.key = key;
	info.type = type;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	infos_.emplace(key, info);
}
