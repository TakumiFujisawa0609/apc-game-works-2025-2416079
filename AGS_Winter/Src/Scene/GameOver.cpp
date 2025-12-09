#include <string>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Input/Controller.h"
#include "GameOver.h"


GameOver::GameOver(void)
{
}

GameOver::~GameOver(void)
{
}

void GameOver::InitLoad(void)
{
}

void GameOver::Init(void)
{
	//img_ = LoadGraph((Application::PATH_IMAGE + "Background.png").c_str());
}

void GameOver::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	Controller& ins = Controller::GetInstance();
	Controller::JOYPAD_IN_STATE state = ins.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (state.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)] || CheckHitKey(KEY_INPUT_SPACE) == 1) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void GameOver::Draw(void)
{
	DrawFormatString(100, 100, 0x000000, "GameOver", SetFontSize(30));
	DrawFormatString(100, 150, 0x000000, "Press A", SetFontSize(30));

	//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, img_, true);
}

void GameOver::Release(void)
{
	//DeleteGraph(img_);
}
