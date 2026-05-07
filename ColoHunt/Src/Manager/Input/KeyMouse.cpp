#include <DxLib.h>
#include "KeyMouse.h"
#include "../../Application.h"


void KeyMouse::Init(void)
{
	// マウス検知
	mouseInput_ = GetMouseInput();

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void KeyMouse::Update(void)
{
	anyone_ = anyoneKey_ = isAnyoneDown_ = false;

	// キーボード検知
	for (auto& p : infos_)
	{
		// キーマウ以外は更新しない
		if (p.second.type != KEYPAD_NO::KEY && p.second.type != KEYPAD_NO::MOUSE) {
			
			continue;
		}
		p.second.keyOld = p.second.keyNew;
		
		// マウスの取得
		if (p.second.type == KEYPAD_NO::MOUSE) {
			p.second.keyNew = CheckMouseKey(p.second.key);
		}
		// キーボードの取得
		else {
			p.second.keyNew = CheckHitKey(p.second.key);
		}

		InputBase::Update(p);
	}
	// マウス検知
	mouseInput_ = GetMouseInput();
	int mouseX = 0;
	int mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	mousePos_.x = static_cast<float>(mouseX) / MOUSE_SEMSITIVITY;
	mousePos_.y = static_cast<float>(mouseY) / MOUSE_SEMSITIVITY;

	// 何も押されていないがマウスが動いているか
	if (mouseX != Application::SCREEN_SIZE_X_HALF && mouseY != Application::SCREEN_SIZE_Y_HALF) {

		anyone_ = true;
	}

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void KeyMouse::Release(void)
{
}

KeyMouse::KeyMouse(int sensi) : mousePos_()
{
	mouseInput_ = -1;
}

bool KeyMouse::CheckMouseKey(int key)
{
	if (mouseInput_ & key) {

		return true;
	}
	return false;
}
