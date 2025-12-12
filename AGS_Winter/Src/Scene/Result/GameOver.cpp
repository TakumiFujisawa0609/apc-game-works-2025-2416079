#include <string>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Input/Controller.h"
#include "GameOver.h"


GameOver::GameOver(void)
{
}

GameOver::~GameOver(void)
{
}

void GameOver::Draw(void)
{
	ResultBase::Draw();

	DrawFormatString(100, 100, 0x000000, "GameOver", SetFontSize(30));

	//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, img_, true);
}

void GameOver::Release(void)
{
	//DeleteGraph(img_);
}
