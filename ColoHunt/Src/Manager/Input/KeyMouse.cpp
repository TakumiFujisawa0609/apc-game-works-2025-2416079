#include <DxLib.h>
#include "KeyMouse.h"
#include "../../Application.h"


void KeyMouse::Init(void)
{
	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void KeyMouse::Update(void)
{

	// キーボード検知
	for (auto& p : infos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = CheckHitKey(p.second.key);
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}

	// マウス検知
	mouseInput_ = GetMouseInput();
	int mouseX = 0;
	int mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	mousePos_.x = static_cast<float>(mouseX);
	mousePos_.y = static_cast<float>(mouseY);

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void KeyMouse::Destroy(void)
{
}

void KeyMouse::Add(int key)
{
	KeyMouse::Info info = KeyMouse::Info();
	info.key = key;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	infos_.emplace(key, info);
}

KeyMouse::KeyMouse(void) : infoEmpty_(), mousePos_()
{
	mouseInput_ = -1;
}

const KeyMouse::Info& KeyMouse::Find(int key) const
{
	auto it = infos_.find(key);
	if (it != infos_.end())
	{
		return it->second;
	}
	return infoEmpty_;
}
