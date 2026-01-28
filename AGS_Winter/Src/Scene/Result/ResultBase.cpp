#include <string>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Input/Controller.h"
#include "../../Manager/Audio/AudioManager.h"
#include "ResultBase.h"


ResultBase::ResultBase(void) : state_(STATE::CONTINUE)
{
}

ResultBase::~ResultBase(void)
{
}

void ResultBase::InitLoad(void)
{
	screenImg_ = SceneManager::GetInstance().GetScreenImage();
}

void ResultBase::Init(void)
{
}

void ResultBase::Update(void)
{
	// シーン遷移
	Controller& ins = Controller::GetInstance();
	Controller::JOYPAD_IN_STATE state = ins.GetJPadState(Controller::JOYPAD_NO::PAD1);
	if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP_DPAD)]) {
		switch (state_) {
		case STATE::CONTINUE:

			state_ = STATE::FINISH;
			break;

		case STATE::RETURN_TITLE:

			state_ = STATE::CONTINUE;
			break;

		case STATE::FINISH:

			state_ = STATE::RETURN_TITLE;
			break;
		}
	}
	else if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN_DPAD)]) {
		switch (state_) {
		case STATE::CONTINUE:

			state_ = STATE::RETURN_TITLE;
			break;

		case STATE::RETURN_TITLE:

			state_ = STATE::FINISH;
			break;

		case STATE::FINISH:

			state_ = STATE::CONTINUE;
			break;
		}
	}

	if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)] ||
		CheckHitKey(KEY_INPUT_RETURN)) {
		switch (state_) {
		case STATE::CONTINUE:

			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			break;

		case STATE::RETURN_TITLE:

			//タイトルへ戻る
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			break;

		case STATE::FINISH:

			//ゲーム終了
			Application::GetInstance().FinishGame();
			break;
		}
	}
}

void ResultBase::Draw(void)
{
	DrawGraph(0, 0, screenImg_, false);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	int dx = Application::SCREEN_SIZE_X / 40;
	int dy = Application::SCREEN_SIZE_Y / 25;

	DrawBox(dx, dy, dx * 6, dy * 10, 0x000000, true);
	DrawLineBox(dx, dy, dx * 6, dy * 10, 0xcaaa00, 5);

	dx += Application::SCREEN_SIZE_X / 15;
	dy += Application::SCREEN_SIZE_Y / 25;
	int width = GetDrawFormatStringWidth("再戦", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "再戦", SetFontSize(30));

	if (state_ == STATE::CONTINUE) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 8;
	width = GetDrawFormatStringWidth("タイトル", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "タイトル", SetFontSize(30));

	if (state_ == STATE::RETURN_TITLE) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 8;
	width = GetDrawFormatStringWidth("終了", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "終了", SetFontSize(30));

	if (state_ == STATE::FINISH) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}
}

void ResultBase::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::RESULT);
	DeleteGraph(screenImg_);
}
