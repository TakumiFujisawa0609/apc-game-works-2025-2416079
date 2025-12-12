#include <string>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Input/Controller.h"
#include "ResultBase.h"


ResultBase::ResultBase(void)
{
}

ResultBase::~ResultBase(void)
{
}

void ResultBase::InitLoad(void)
{
}

void ResultBase::Init(void)
{
	resultImg_ = SceneManager::GetInstance().GetResultImage();
}

void ResultBase::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	Controller& ins = Controller::GetInstance();
	Controller::JOYPAD_IN_STATE state = ins.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (state.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)] || CheckHitKey(KEY_INPUT_SPACE) == 1) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void ResultBase::Draw(void)
{
	DrawGraph(0, 0, resultImg_, false);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawFormatString(100, 150, 0xffffff, "Press A", SetFontSize(30));

	//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgBg_, true);
	//DrawRotaGraph( Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0f, 0.0, imgTitle_, true);
}

void ResultBase::Release(void)
{
}
