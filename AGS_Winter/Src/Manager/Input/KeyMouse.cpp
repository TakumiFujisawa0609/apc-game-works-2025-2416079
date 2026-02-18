#include <DxLib.h>
#include "KeyMouse.h"

KeyMouse* KeyMouse::instance_ = nullptr;

void KeyMouse::CreateInstance(void)
{
	instance_ = new KeyMouse();

	instance_->Init();
}

KeyMouse& KeyMouse::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		KeyMouse::CreateInstance();
	}
	return *instance_;
}

void KeyMouse::Init(void)
{

	// ゲームで使用したいキーを、
	// 事前にここで登録しておいてください
	KeyMouse::GetInstance().Add(KEY_INPUT_SPACE);
	KeyMouse::GetInstance().Add(KEY_INPUT_N);
	KeyMouse::GetInstance().Add(KEY_INPUT_Z);

	KeyMouse::GetInstance().Add(KEY_INPUT_W);
	KeyMouse::GetInstance().Add(KEY_INPUT_A);
	KeyMouse::GetInstance().Add(KEY_INPUT_S);
	KeyMouse::GetInstance().Add(KEY_INPUT_D);

	KeyMouse::GetInstance().Add(KEY_INPUT_UP);
	KeyMouse::GetInstance().Add(KEY_INPUT_DOWN);
	KeyMouse::GetInstance().Add(KEY_INPUT_LEFT);
	KeyMouse::GetInstance().Add(KEY_INPUT_RIGHT);

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
	// インスタンスのメモリ解放
	delete instance_;
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

void KeyMouse::Clear(void)
{
	keyInfos_.clear();
}

bool KeyMouse::IsNew(int key) const
{
	return Find(key).keyNew;
}

bool KeyMouse::IsTrgDown(int key) const
{
	return Find(key).keyTrgDown;
}

bool KeyMouse::IsTrgUp(int key) const
{
	return Find(key).keyTrgUp;
}

VECTOR KeyMouse::GetMousePos(void) const
{
	return mousePos_;
}

int KeyMouse::GetMouse(void) const
{
	return mouseInput_;
}

bool KeyMouse::IsClickMouseLeft(void) const
{
	return mouseInput_ == MOUSE_INPUT_LEFT;
}

bool KeyMouse::IsClickMouseRight(void) const
{
	return mouseInput_ == MOUSE_INPUT_RIGHT;
}

bool KeyMouse::IsTrgMouseLeft(void) const
{
	return FindMouse(MOUSE_INPUT_LEFT).keyTrgDown;
}

bool KeyMouse::IsTrgMouseRight(void) const
{
	return FindMouse(MOUSE_INPUT_RIGHT).keyTrgDown;
}

KeyMouse::KeyMouse(void)
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
