#include <DxLib.h>
#include "KeyMouse.h"
#include "../../Application.h"


void KeyMouse::Init(void)
{
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false;

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void KeyMouse::Update(void)
{
	// 初期化
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false; 
	mouseInput_ = GetMouseInput();

	// キーボード検知
	for (auto& p : infos_)
	{
		// キーマウ以外は更新しない
		if (p.second.type != KEYPAD_NO::KEY && p.second.type != KEYPAD_NO::MOUSE) {
			
			continue;
		}
		p.second.keyOld = p.second.keyNew;
		if (p.second.type == TYPE::KEY) {
			p.second.keyNew = CheckHitKey(p.second.key);
		}
		if (p.second.type == TYPE::MOUSE) {
			p.second.keyNew = CheckMouse(p.second.key);
		}
		InputBase::Update(p);
	}

		InputBase::Update(p);
	}
	// マウス検知
	int mouseX = 0;
	int mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	mousePos_.x = static_cast<float>(mouseX) / MOUSE_SEMSITIVITY;
	mousePos_.y = static_cast<float>(mouseY) / MOUSE_SEMSITIVITY;

	// 何も押されていないがマウスが動いているか
	if (mouseX != Application::SCREEN_SIZE_X_HALF && mouseY != Application::SCREEN_SIZE_Y_HALF) {

		anyone_ = true;
	}

	if (!anyone_ && mousePos_.x != Application::SCREEN_SIZE_X_HALF && mousePos_.y != Application::SCREEN_SIZE_Y_HALF) {

		anyone_ = true;
	}

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void KeyMouse::Release(void)
{
}

bool KeyMouse::CheckMouse(int key)
{
	if (mouseInput_ & key) {

		return true;
	}
	return false;
}

KeyMouse::KeyMouse(void) : mousePos_()
{
	mouseInput_ = -1;
}
