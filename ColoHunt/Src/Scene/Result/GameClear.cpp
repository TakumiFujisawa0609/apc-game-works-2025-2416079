#include <string>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Input/Controller.h"
#include "../../Manager/Audio/AudioManager.h"
#include "GameClear.h"


GameClear::GameClear(void)
{
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
	timeHandle = CreateFontToHandle("Monserhunterfonts Xtype", 250, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	scoreHandle = CreateFontToHandle("MSPゴシック", 80, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	AudioManager::GetInstance()->SetBgmVolume(200);
}

void GameClear::Draw(void)
{
	ResultBase::Draw();

	int time = SceneManager::GetInstance().GetTime();
	DrawFormatStringToHandle(600, 300, 0xffffff, timeHandle, "%d:%02d", time / 60, time % 60);
	DrawFormatStringToHandle(550, 650, 0xffffff, scoreHandle, "   被 弾 回 数   : %d", SceneManager::GetInstance().GetDamage());
	DrawFormatStringToHandle(550, 750, 0xffffff, scoreHandle, "アイテム使用回数 : %d", SceneManager::GetInstance().GetItem());
}

void GameClear::Release(void)
{
	//DeleteGraph(imgTitle_);
	//DeleteGraph(imgBg_);
}
