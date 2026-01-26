#include <string>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Audio/AudioManager.h"
#include "../Manager/Audio/SoundTable.h"
#include "../Manager/Input/Controller.h"
#include "TitleScene.h"


TitleScene::TitleScene(void) : SceneBase(), cnt_(0), show_(true)
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
	font_ = CreateFontToHandle("アンニャントロマン", 45, 1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	creatorFont_ = CreateFontToHandle("MS 明朝", 15, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void TitleScene::Update(void)
{
	cnt_++;

	if(cnt_ >= CHANGE_CNT) {
		
		show_ = !show_;
		cnt_ = 0;
	}

	// シーン遷移
	Controller& ins = Controller::GetInstance();
	Controller::JOYPAD_IN_STATE state = ins.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (state.IsAnyoneDown || CheckHitKey(KEY_INPUT_SPACE) == 1) {
		if (state.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::START)]) return;

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	SceneBase::Draw();

	int widthA = GetDrawFormatStringWidthToHandle(font_, "Press Any Botton");

	if (show_) {
		
		DrawFormatStringToHandle((Application::SCREEN_SIZE_X - widthA) / 2, Application::SCREEN_SIZE_Y - 175, 0xdd2d2d, font_, "Press Any Botton");
	}

	int widthB = GetDrawFormatStringWidthToHandle(creatorFont_, "HIRO 2025,2026 CREATED");

	DrawFormatStringToHandle((Application::SCREEN_SIZE_X - widthB) / 2, Application::SCREEN_SIZE_Y - 100, 0xffffff, creatorFont_, "HIRO 2025,2026 CREATED");

	//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgBg_, true);
	DrawRotaGraph( Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 0.75f, 0.0, imgTitle_, true);
}

void TitleScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);
	DeleteGraph(imgTitle_);
	DeleteFontToHandle(font_);
	DeleteFontToHandle(creatorFont_);
	//DeleteGraph(imgBg_);
}
