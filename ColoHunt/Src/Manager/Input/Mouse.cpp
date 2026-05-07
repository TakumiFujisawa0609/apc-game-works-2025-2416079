#include <DxLib.h>
#include "Mouse.h"
#include "../../Application.h"


void Mouse::Init(void)
{
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false;

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void Mouse::Update(void)
{
	// 初期化
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false;
	mouseInput_ = GetMouseInput();

	// キーボード検知
	for (auto& p : infos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = CheckMouse(p.second.key);
		InputBase::Update(p);
	}

	// マウス検知
	int mouseX = 0;
	int mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	mousePos_.x = static_cast<float>(mouseX);
	mousePos_.y = static_cast<float>(mouseY);

	if (!anyone_ && mousePos_.x != Application::SCREEN_SIZE_X_HALF && mousePos_.y != Application::SCREEN_SIZE_Y_HALF) {

		anyone_ = true;
	}

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void Mouse::Release(void)
{
}

bool Mouse::CheckMouse(int key)
{
	if (mouseInput_ & key) {

		return true;
	}
	return false;
}

Mouse::Mouse(void) : mousePos_()
{
	mouseInput_ = -1;
}
