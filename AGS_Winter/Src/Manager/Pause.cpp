#include "Pause.h"
#include <DxLib.h>
#include "../Manager/Input/Controller.h"
#include "../Utility/Utility.h"
#include "../Application.h"


Pause::Pause() : prevId_(SceneManager::SCENE_ID::NONE), isPause_(false), state_(PAUSE_STATE::CONTINUE), image_(-1), pos_(Utility::VECTOR_ZERO)
{
}

Pause::~Pause()
{
}

void Pause::Init(SceneManager::SCENE_ID id)
{
	prevId_ = id;
	isPause_ = true;
	state_ = PAUSE_STATE::CONTINUE;
	pos_.y = Application::SCREEN_SIZE_Y;

	if (image_ == -1) {
	
		image_ = LoadGraph((Application::PATH_IMAGE + "Arrow.png").c_str());
	}
}

void Pause::Update()
{
	if (pos_.y > 0) {

		pos_.y -= 95;

		if (pos_.y <= 0) {

			pos_.y = 0;
		}
	}
	else {
		if (prevId_ == SceneManager::SCENE_ID::TITLE) {
			if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT_DPAD)] ||
				Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::LEFT_DPAD)] ||
				CheckHitKey(KEY_INPUT_T)) {
				switch (state_) {
				case Pause::PAUSE_STATE::CONTINUE:

					state_ = Pause::PAUSE_STATE::FINISH;
					break;

				case Pause::PAUSE_STATE::FINISH:

					state_ = Pause::PAUSE_STATE::CONTINUE;
					break;
				}
			}
		}
		else {
			if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP_DPAD)]) {
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
		}
		if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)] ||
			CheckHitKey(KEY_INPUT_RETURN)) {
			switch (state_) {
			case Pause::PAUSE_STATE::CONTINUE:

				isPause_ = false;
				break;

			case Pause::PAUSE_STATE::SETTING:

				//設定画面へ
				break;

			case Pause::PAUSE_STATE::RETURN_TITLE:

				//タイトルへ戻る
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
				isPause_ = false;
				break;

			case Pause::PAUSE_STATE::FINISH:

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

	if (prevId_ == SceneManager::SCENE_ID::TITLE) {

		int dx = Application::SCREEN_SIZE_X / 4;
		int dy = Application::SCREEN_SIZE_Y / 4 - pos_.y;
		
		DrawBox(dx, dy, dx * 3, dy * 3, 0x000000, true);
		DrawLineBox(dx, dy, dx * 3, dy * 3, 0xcaaa00, 5);

		dx += Application::SCREEN_SIZE_X / 4;
		dy += Application::SCREEN_SIZE_Y / 12;

		int width = GetDrawFormatStringWidth("ゲームを終了しますか", SetFontSize(40)) / 2;

		DrawFormatString(dx - width, dy, 0xffffff, "ゲームを終了しますか", SetFontSize(40));

		dx -= Application::SCREEN_SIZE_X / 6;
		dy += Application::SCREEN_SIZE_Y / 3;
		
		width = GetDrawFormatStringWidth("はい", SetFontSize(30)) / 2;

		DrawFormatString(dx - width, dy, 0xffffff, "はい", SetFontSize(30));

		dx += Application::SCREEN_SIZE_X / 3;
		width = GetDrawFormatStringWidth("いいえ", SetFontSize(30)) / 2;

		DrawFormatString(dx - width, dy, 0xffffff, "いいえ", SetFontSize(30));

		dx = Application::SCREEN_SIZE_X / 3 * (static_cast<int>(state_) % 4) - 65;
		dy += 15;

		DrawTriangle(dx, dy, dx - 20, dy - 20, dx - 20, dy + 20, 0xffffff, true);
	}
	else {
		int dx = Application::SCREEN_SIZE_X / 3;
		int dy = Application::SCREEN_SIZE_Y / 7 - pos_.y;

		DrawBox(dx, dy, dx * 2, dy *  25 / 4 , 0x000000, true);
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
	}
}

void Pause::Release()
{
	DeleteGraph(image_);
}
