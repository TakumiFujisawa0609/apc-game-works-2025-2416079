#include "Pause.h"
#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/Input/InputManager.h"
#include "../Utility/Utility.h"
#include "../Application.h"


Pause::Pause() : state_(STATE::CONTINUE), pos_(Utility::VECTOR_ZERO), pauseBGM_()
{
}

Pause::~Pause()
{
}

void Pause::Init(void)
{
	state_ = STATE::CONTINUE;
	pos_.y = Application::SCREEN_SIZE_Y;
	pauseBGM_ = AudioManager::GetInstance()->PauseBGM();
	AudioManager::GetInstance()->StopSE();
}

void Pause::Update()
{
	// 枠がまだ上にある
	if (pos_.y > 0) {

		// 枠を上からおろす
		pos_.y -= DOWN;

		if (pos_.y <= 0) {

			pos_.y = 0;
		}
	}
	// 枠が止まってから動く
	else {
		// 今の矢印の位置をintに
		int i = static_cast<int>(state_);

		//上を押したとき
		if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::UP).keyTrgDown) {

			AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);
			
			// 上に行く
			i--;

			// 一番上で上を押した時
			if (i < static_cast<int>(STATE::CONTINUE)) {

				i = static_cast<int>(STATE::CONTINUE);
			}
		}
		//下を押したとき
		else if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DOWN).keyTrgDown) {

			AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);

			// 下に行く
			i++;

			// 一番下で下を押したとき
			if (i > static_cast<int>(STATE::FINISH)) {

				i = static_cast<int>(STATE::FINISH);
			}
		}
		// intを変える
		state_ = static_cast<STATE>(i);

		// 決定したとき
		if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DECIDE).keyTrgDown) {
		
			//カーソルの位置で
			switch (state_) {
			case Pause::STATE::CONTINUE:

				AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);
				AudioManager::GetInstance()->PlayBGM(pauseBGM_);
				//ゲームへ戻る
				SceneManager::GetInstance().PopScene();
				break;

			case Pause::STATE::RETURN_TITLE:

				AudioManager::GetInstance()->PlaySE(SoundID::SE_DESIDE);
				//タイトルへ戻る
				SceneManager::GetInstance().JumpScene(SceneManager::SCENE_ID::TITLE);
				break;

			case Pause::STATE::FINISH:

				AudioManager::GetInstance()->PlaySE(SoundID::SE_DESIDE);
				//ゲーム終了
				Application::GetInstance().FinishGame();
				break;
			}
		}
		// もう一度押しても
		if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::PAUSE).keyTrgDown) {

			AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);
			AudioManager::GetInstance()->PlayBGM(pauseBGM_);
			//ゲームへ戻る
			SceneManager::GetInstance().PopScene();
		}
	}
}

void Pause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	int dx = Application::SCREEN_SIZE_X / 3;
	int dy = Application::SCREEN_SIZE_Y / 7 - (int)pos_.y;

	DrawBox(dx, dy, dx * 2, dy * 25 / 4, 0x000000, true);
	DrawLineBox(dx, dy, dx * 2, dy * 25 / 4, 0xcaaa00, 5);

	dx += Application::SCREEN_SIZE_X / 6;
	dy += Application::SCREEN_SIZE_Y / 38;

	int width = GetDrawFormatStringWidth("一時停止中", SetFontSize(40)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "一時停止中", SetFontSize(40));

	dy += Application::SCREEN_SIZE_Y / 5;
	width = GetDrawFormatStringWidth("再開", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "再開", SetFontSize(30));

	if (state_ == STATE::CONTINUE) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 5;
	width = GetDrawFormatStringWidth("リタイア", SetFontSize(30)) / 2;

	DrawFormatString(dx - width, dy, 0xffffff, "リタイア", SetFontSize(30));

	if (state_ == STATE::RETURN_TITLE) {

		DrawTriangle(dx - 65, dy + 15, dx - 90, dy, dx - 90, dy + 30, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 5;
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
	else if (InputManager::GetInstance().GetMostPriorityType() <= InputBase::JOYPAD_TYPE::XBOX_ONE) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Ａ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Ｂ　戻る", 0xffffff);
	}
	else if (InputManager::GetInstance().GetMostPriorityType() <= InputBase::JOYPAD_TYPE::DUAL_SENSE) {

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
