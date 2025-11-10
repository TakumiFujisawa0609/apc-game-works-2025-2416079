#include<DxLib.h>
#include "Item.h"
#include"../Application.h"
#include "../Manager/Controller.h"


Item::Item(void)
{
}

Item::~Item(void)
{
}

void Item::Init(void)
{
	for (int i = 0; i < static_cast<int>(TYPE::MAX); i++) {

		switch (i)		{
		case 0:

			itemImg_[0] = LoadGraph((Application::PATH_IMAGE + "HP.png").c_str());
			itemNum_[0] = 10;
			break;

		case 1:

			itemImg_[1] = LoadGraph((Application::PATH_IMAGE + "HP_Max.png").c_str());
			itemNum_[1] = 2;
			break;

		case 2:

			itemImg_[2] = LoadGraph((Application::PATH_IMAGE + "Stamina.png").c_str());
			itemNum_[2] = 5;
			break;
		}
	}
	itemNo_ = 0;
}

void Item::Update(void)
{
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT_DPAD)]) {

		itemNo_++;

		if (itemNo_ >= static_cast<int>(TYPE::MAX)) {

			itemNo_ = 0;
		}
	}
	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::LEFT_DPAD)]) {

		itemNo_--;
	
		if (itemNo_ <1 0) {

			itemNo_ = static_cast<int>(TYPE::MAX) - 1;
		}
	}
}

void Item::Draw(void)
{
	int dx = Application::SCREEN_SIZE_X - 100;
	int dy = Application::SCREEN_SIZE_Y - 100;

	DrawBoxAA(dx - 40.0f, dy - 40.0f, dx + 40.0f, dy + 40.0f, 0x222222, true);
	DrawRotaGraph(dx, dy, 1.0, 0.0, itemImg_[itemNo_], true);
}

void Item::Release(void)
{
	for (int i = static_cast<int>(TYPE::MAX) - 1; i >= 0; i--) {

		DeleteGraph(itemImg_[i]);
	}
}
