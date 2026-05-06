#include "InputBase.h"


InputBase::InputBase(void)
{
}

void InputBase::Update(std::pair<const int, Info> p)
{
	p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
	p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;

	if (!anyoneTrgDown_) {
		anyoneTrgDown_ = p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		anyoneTrg_ = p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		anyone_ = p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
	}
	if (!anyoneTrg_) {
		anyoneTrg_ = p.second.keyTrgDown = p.second.keyNew;
		anyone_ = p.second.keyTrgDown = p.second.keyNew;
	}
}

void InputBase::Add(int key, TYPE type)
{
	Info info = Info();
	info.key = key;
	info.type = type;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	infos_.emplace(key, info);
}
