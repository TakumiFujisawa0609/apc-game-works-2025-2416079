#include "Pause.h"
#include <DxLib.h>
#include "../Manager/Input/Controller.h"
#include "../Manager/Audio/AudioManager.h"
#include "../Utility/Utility.h"
#include "../Application.h"


Pause::Pause() : isPause_(false), state_(PAUSE_STATE::CONTINUE), pos_(Utility::VECTOR_ZERO)
{
}

Pause::~Pause()
{
}

void Pause::Init(void)
{
	isPause_ = true;
	state_ = PAUSE_STATE::CONTINUE;
	pos_.y = Application::SCREEN_SIZE_Y;
}

void Pause::Update()
{
	if (pos_.y > 0) {

		pos_.y -= DOWN;

		if (pos_.y <= 0) {

			pos_.y = 0;
		}
	}
	else {
		if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP_DPAD)]) {

			AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);

			switch (state_) {
			case Pause::PAUSE_STATE::CONTINUE:

				state_ = Pause::PAUSE_STATE::FINISH;
				break;

			case Pause::PAUSE_STATE::SETTING:

				state_ = Pause::PAUSE_STATE::CONTINUE;
				break;

			case Pause::PAUSE_STATE::RETURN_TITLE:

				state_ = Pause::PAUSE_STATE::SETTING;
				break;

			case Pause::PAUSE_STATE::FINISH:

				state_ = Pause::PAUSE_STATE::RETURN_TITLE;
				break;
			}
		}
		else if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN_DPAD)]) {

			AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);

			switch (state_) {
			case Pause::PAUSE_STATE::CONTINUE:

				state_ = Pause::PAUSE_STATE::SETTING;
				break;

			case Pause::PAUSE_STATE::SETTING:

				state_ = Pause::PAUSE_STATE::RETURN_TITLE;
				break;

			case Pause::PAUSE_STATE::RETURN_TITLE:

				state_ = Pause::PAUSE_STATE::FINISH;
				break;

			case Pause::PAUSE_STATE::FINISH:

				state_ = Pause::PAUSE_STATE::CONTINUE;
				break;
			}
		}
		if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {
			switch (state_) {
			case Pause::PAUSE_STATE::CONTINUE:

				AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);
				isPause_ = false;
				break;

			case Pause::PAUSE_STATE::SETTING:

				//設定画面へ
				break;

			case Pause::PAUSE_STATE::RETURN_TITLE:

				AudioManager::GetInstance()->PlaySE(SoundID::SE_DESIDE);
				//タイトルへ戻る
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
				isPause_ = false;
				break;

			case Pause::PAUSE_STATE::FINISH:

				AudioManager::GetInstance()->PlaySE(SoundID::SE_DESIDE);
				//ゲーム終了
				Application::GetInstance().FinishGame();
				break;
			}
		}
	}
}

void Pause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	int dx = Application::SCREEN_SIZE_X / 3;
	int dy = Application::SCREEN_SIZE_Y / 7 - pos_.y;

	DrawBox(dx, dy, dx * 2, dy * 25 / 4, 0x000000, true);
	DrawLineBox(dx, dy, dx * 2, dy * 25 / 4, 0xcaaa00, 5);

	dx += Application::SCREEN_SIZE_X / 6;
	dy += Application::SCREEN_SIZE_Y / 42;

	int width = GetDrawFormatStringWidth("一時停止中", SetFontSize(40)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "一時停止中", SetFontSize(40));

	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("再開", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "再開", SetFontSize(30));

	if (state_ == PAUSE_STATE::CONTINUE) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("設定", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "設定", SetFontSize(30));
	DrawLine(dx - width - 10, dy + 10, dx + width + 10, dy + 10, 0xffffff, 2);
	DrawLine(dx - width - 10, dy + 20, dx + width + 10, dy + 20, 0xffffff, 2);

	if (state_ == PAUSE_STATE::SETTING) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("リタイア", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "リタイア", SetFontSize(30));

	if (state_ == PAUSE_STATE::RETURN_TITLE) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("終了", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "終了", SetFontSize(30));

	if (state_ == PAUSE_STATE::FINISH) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}


	//文字サイズを得る
	int fontSize = GetFontSize();
	//サイズをあげる
	SetFontSize(20);

	//表示
	DrawBox(0, Application::SCREEN_SIZE_Y - 50, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_X, 0x000000, true);
	DrawString(100, Application::SCREEN_SIZE_Y - 40, "↑↓　選択", 0xffffff);

	//コントローラータイプに応じて表示を変える
	if (Controller::GetInstance().GetJPadType(Controller::JOYPAD_NO::PAD1) <= Controller::JOYPAD_TYPE::XBOX_ONE) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Ａ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Ｂ　戻る", 0xffffff);
	}
	else if (Controller::GetInstance().GetJPadType(Controller::JOYPAD_NO::PAD1) <= Controller::JOYPAD_TYPE::DUAL_SENSE) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "×　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "○　戻る", 0xffffff);
	}
	else {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Ｂ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Ａ　戻る", 0xffffff);
	}

	//サイズを戻す
	SetFontSize(fontSize);
}

void Pause::Release()
{
}
