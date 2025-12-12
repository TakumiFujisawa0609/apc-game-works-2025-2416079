#include <string>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Input/Controller.h"
#include "GameClear.h"


GameClear::GameClear(void)
{
}

GameClear::~GameClear(void)
{
}

void GameClear::Draw(void)
{
	ResultBase::Draw();

	DrawFormatString(100, 100, 0x000000, "GameClear", SetFontSize(30));

	//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgBg_, true);
	//DrawRotaGraph( Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0f, 0.0, imgTitle_, true);
}

void GameClear::Release(void)
{
	//DeleteGraph(imgTitle_);
	//DeleteGraph(imgBg_);
}
