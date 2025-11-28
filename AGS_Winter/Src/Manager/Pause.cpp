#include "Pause.h"
#include <DxLib.h>
#include "../Manager/Input/Controller.h"
#include "../Application.h"


Pause::Pause() : prevId_(SceneManager::SCENE_ID::NONE), isPause_(false), state_(PAUSE_STATE::CONTINUE), image_(-1)
{
}

Pause::~Pause()
{
}

void Pause::Init(SceneManager::SCENE_ID id)
{
	prevId_ = id;
	isPause_ = true;

	if (image_ == -1) {
	
		image_ = LoadGraph((Application::PATH_IMAGE + "Arrow.png").c_str());
	}
}

void Pause::Update()
{
	if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP_DPAD)]) {
		switch (state_){
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
		switch (state_){
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

			isPause_ = false;
			break;

		case Pause::PAUSE_STATE::SETTING:

			//設定画面へ
			isPause_ = false;
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

void Pause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	int dx = Application::SCREEN_SIZE_X / 2;
	int dy = Application::SCREEN_SIZE_Y / 6;
	int width = GetDrawFormatStringWidth("一時停止中");

	DrawFormatString(dx - width, dy, 0xffffff, "一時停止中", SetFontSize(40));
	
	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("再開") / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "再開", SetFontSize(30));
	
	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("設定") / 2;
	
	DrawFormatString(dx - width, dy, 0xffffff, "設定", SetFontSize(30));
	DrawLine(dx - width - 10, dy + 10, dx + width + 10, dy + 10, 0xffffff, 2);
	DrawLine(dx - width - 10, dy + 20, dx + width + 10, dy + 20, 0xffffff, 2);
	
	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("リタイア") / 2;
	
	DrawFormatString(dx - width, dy, 0xffffff, "リタイア", SetFontSize(30));
		
	dy += Application::SCREEN_SIZE_Y / 6;
	width = GetDrawFormatStringWidth("終了") / 2;
	
	DrawFormatString(dx - width, dy, 0xffffff, "終了", SetFontSize(30));

	DrawRotaGraph(dx - 100.0f, Application::SCREEN_SIZE_Y / 6 * static_cast<float>(static_cast<int>(state_)) + 10, 1.0, 0.0, image_, true);
}

void Pause::Release()
{
	DeleteGraph(image_);
}
