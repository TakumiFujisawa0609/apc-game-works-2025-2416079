#include <string>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Input/InputManager.h"
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
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::UP).keyTrgDown) {
		
		AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);

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
	else if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DOWN).keyTrgDown) {
		
		AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);

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

	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DECIDE).keyTrgDown) {
		
		AudioManager::GetInstance()->PlaySE(SoundID::SE_DESIDE);

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
	//黒背景
	DrawGraph(0, 0, screenImg_, false);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//描画
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

	//文字サイズを得る
	int fontSize = GetFontSize();
	//サイズをあげる
	SetFontSize(20);

	//表示
	DrawBox(0, Application::SCREEN_SIZE_Y - 50, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_X, 0x000000, true);
	DrawString(100, Application::SCREEN_SIZE_Y - 40, "↑↓　選択", 0xffffff);

	//コントローラータイプに応じて表示を変える
	if (InputManager::GetInstance().GetMostPriorityType() == InputBase::JOYPAD_TYPE::NON) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Lｸﾘｯｸ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Rｸﾘｯｸ　戻る", 0xffffff);
	}
	else if (InputManager::GetInstance().GetMostPriorityType() <= Controller::JOYPAD_TYPE::XBOX_ONE) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Ａ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Ｂ　戻る", 0xffffff);
	}
	else if (InputManager::GetInstance().GetMostPriorityType() <= Controller::JOYPAD_TYPE::DUAL_SENSE) {

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

void ResultBase::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::RESULT);
	DeleteGraph(screenImg_);
}
