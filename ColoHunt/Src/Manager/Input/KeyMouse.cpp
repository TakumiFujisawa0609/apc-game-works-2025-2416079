#include <DxLib.h>
#include "KeyMouse.h"
#include "../../Application.h"


void KeyMouse::Init(void)
{
	KeyMouse::MouseInfo info;

	// 左クリック
	info = KeyMouse::MouseInfo();
	info.key = MOUSE_INPUT_LEFT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	// 右クリック
	info = KeyMouse::MouseInfo();
	info.key = MOUSE_INPUT_RIGHT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void KeyMouse::Update(void)
{

	// キーボード検知
	for (auto& p : keyInfos_)
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

	for (auto& p : mouseInfos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = mouseInput_ == p.second.key;
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}
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
	keyInfos_.emplace(key, info);
}

KeyMouse::KeyMouse(void) : infoEmpty_(), mouseInfoEmpty_(), mousePos_()
{
	mouseInput_ = -1;
}

const KeyMouse::Info& KeyMouse::Find(int key) const
{

	auto it = keyInfos_.find(key);
	if (it != keyInfos_.end())
	{
		return it->second;
	}
	return infoEmpty_;
}

const KeyMouse::MouseInfo& KeyMouse::FindMouse(int key) const
{
	auto it = mouseInfos_.find(key);
	if (it != mouseInfos_.end())
	{
		return it->second;
	}
	return mouseInfoEmpty_;
}
