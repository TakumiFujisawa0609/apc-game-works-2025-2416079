<<<<<<< HEAD
#include<DxLib.h>
#include "Item.h"
#include"../Application.h"
#include "../Manager/Controller.h"
=======
#include <DxLib.h>
#include "Item.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
>>>>>>> dc1217fbb6e215f6d79dfc738caff453d3bc6f4c


Item::Item(void)
{
}

Item::~Item(void)
{
}

void Item::Init(void)
{
<<<<<<< HEAD
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
=======
	for (int i = 0; i < static_cast<int>(TYPE::MAX); i++){
				
		itemImg_[i] = LoadGraph((Application::PATH_IMAGE + "item_" + std::to_string(i) + ".png").c_str());

		switch (i)
		{
			case 0:

			itemNum_[i] = 10;
			break;

			case 1:

			itemNum_[i] = 2;
			break;

			case 2:

			itemNum_[i] = 5;
			break;
		}
	}
	type_ = TYPE::HP_MAX;
	use_ = true;
>>>>>>> dc1217fbb6e215f6d79dfc738caff453d3bc6f4c
}

void Item::Update(void)
{
<<<<<<< HEAD
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
=======
	prevKey_ = nowKey_;
	nowKey_ = CheckHitKey(KEY_INPUT_SPACE);

	use_ = false;

	if (using_) {

		return;
	}
	if (nowKey_ == 1 && prevKey_ == 0) {
		if (itemNum_[static_cast<int>(type_)] > 0) {

			use_ = true;
			itemNum_[static_cast<int>(type_)]--;
>>>>>>> dc1217fbb6e215f6d79dfc738caff453d3bc6f4c
		}
	}
}

void Item::Draw(void)
{
<<<<<<< HEAD
	int dx = Application::SCREEN_SIZE_X - 100;
	int dy = Application::SCREEN_SIZE_Y - 100;

	DrawBoxAA(dx - 40.0f, dy - 40.0f, dx + 40.0f, dy + 40.0f, 0x222222, true);
	DrawRotaGraph(dx, dy, 1.0, 0.0, itemImg_[itemNo_], true);
=======
	float dx = Application::SCREEN_SIZE_X - 100.0f;
	float dy = Application::SCREEN_SIZE_Y - 100.0f;

	DrawFormatString(dx - 40.0f, dy + 40.0f, 0x000000, "%d", itemNum_[static_cast<int>(type_)]);
	DrawBoxAA(dx - 50.0f, dy - 50.0f, dx + 50.0f, dy + 50.0f, 0x222222, true);
	DrawRotaGraph(dx, dy, 1.0f, 0.0f, itemImg_[static_cast<int>(type_)], true);
>>>>>>> dc1217fbb6e215f6d79dfc738caff453d3bc6f4c
}

void Item::Release(void)
{
<<<<<<< HEAD
	for (int i = static_cast<int>(TYPE::MAX) - 1; i >= 0; i--) {
=======
	for (int i = static_cast<int>(TYPE::MAX) -1; i >= 0; i--){
>>>>>>> dc1217fbb6e215f6d79dfc738caff453d3bc6f4c

		DeleteGraph(itemImg_[i]);
	}
}
