#include <string>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Audio/AudioManager.h"
#include "../Manager/Audio/SoundTable.h"
#include "../Manager/Input/Controller.h"
#include "TitleScene.h"


TitleScene::TitleScene(void)
{
	imgTitle_ = -1;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::InitLoad(void)
{
	imgTitle_ = LoadGraph((Application::PATH_IMAGE + "Title.png").c_str());
}

void TitleScene::Init(void)
{
	//imgBg_ = LoadGraph((Application::PATH_IMAGE + "Background.png").c_str());
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::TITLE);
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_TITLE);
}

void TitleScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	Controller& ins = Controller::GetInstance();
	Controller::JOYPAD_IN_STATE state = ins.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (state.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)] || CheckHitKey(KEY_INPUT_SPACE) == 1) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	SceneBase::Draw();

	DrawFormatString(100, 100, 0x000000, "Title", SetFontSize(30));
	DrawFormatString(100, 150, 0x000000, "Press A", SetFontSize(30));

	//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgBg_, true);
	DrawRotaGraph( Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgTitle_, true);
}

void TitleScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);
	DeleteGraph(imgTitle_);
	//DeleteGraph(imgBg_);
}
