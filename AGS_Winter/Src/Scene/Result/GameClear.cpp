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

void GameClear::Init(void)
{
	timeHandle = CreateFontToHandle("Monserhunterfonts Xtype", 200, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void GameClear::Draw(void)
{
	ResultBase::Draw();

	int time = SceneManager::GetInstance().GetTime() / 1000;
	DrawFormatStringToHandle(550, 300, 0xffffff, timeHandle, "%d:%02d", time / 60, time % 60);
}

void GameClear::Release(void)
{
	//DeleteGraph(imgTitle_);
	//DeleteGraph(imgBg_);
}
