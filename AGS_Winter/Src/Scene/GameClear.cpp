#include <string>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Input/Controller.h"
#include "GameClear.h"


GameClear::GameClear(void)
{
	imgTitle_ = -1;
}

GameClear::~GameClear(void)
{
}

void GameClear::InitLoad(void)
{
}

void GameClear::Init(void)
{
	//imgTitle_ = LoadGraph((Application::PATH_IMAGE + "Title.png").c_str());
	//imgBg_ = LoadGraph((Application::PATH_IMAGE + "Background.png").c_str());
}

void GameClear::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	Controller& ins = Controller::GetInstance();
	Controller::JOYPAD_IN_STATE state = ins.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (state.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)] || CheckHitKey(KEY_INPUT_SPACE) == 1) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void GameClear::Draw(void)
{
	SceneBase::Draw();

	DrawFormatString(100, 100, 0x000000, "GameClear", SetFontSize(30));
	DrawFormatString(100, 150, 0x000000, "Press A", SetFontSize(30));

	//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgBg_, true);
	//DrawRotaGraph( Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0f, 0.0, imgTitle_, true);
}

void GameClear::Release(void)
{
	//DeleteGraph(imgTitle_);
	//DeleteGraph(imgBg_);
}
