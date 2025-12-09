#include<DxLib.h>
#include "Item.h"
#include"../Application.h"
#include "../Manager/Input/Controller.h"
#include "../Utility/AsoUtility.h"


Item::Item(void)
{
}

Item::~Item(void)
{
}

void Item::InitLoad(void)
{
	for (int i = 0; i < static_cast<int>(TYPE::MAX); i++) {

		itemImg_[i] = LoadGraph((Application::PATH_IMAGE + "item_" + std::to_string(i) + ".png").c_str());
	}
}

void Item::Init(void)
{
	for (int i = 0; i < static_cast<int>(TYPE::MAX); i++) {

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
	type_ = TYPE::HP;
	use_ = true;
}

void Item::Update(void)
{
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT_DPAD)]) {

		int itemNo = static_cast<int>(type_);
		int prevNo = itemNo;
		itemNo++;

		if (itemNo >= static_cast<int>(TYPE::MAX)) {

			itemNo = 0;
		}
		while (itemNum_[itemNo] <= 0) {

			itemNo++;
			if (itemNo >= static_cast<int>(TYPE::MAX)) {

				itemNo = 0;
			}
			if (itemNo == prevNo) {

				break;
			}
		}
		type_ = static_cast<TYPE>(itemNo);
	}
	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::LEFT_DPAD)]) {
		
		int itemNo = static_cast<int>(type_);
		int prevNo = itemNo;
		itemNo--;

		if (itemNo < 0) {

			itemNo = static_cast<int>(TYPE::MAX) - 1;
		}
		while (itemNum_[itemNo] <= 0) {

			itemNo--;
			if (itemNo < 0) {

				itemNo = static_cast<int>(TYPE::MAX) - 1;
			}
			if (itemNo == prevNo) {

				break;
			}
		}
		type_ = static_cast<TYPE>(itemNo);
	}

	use_ = false;

	if (using_) {

		return;
	}
	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::LEFT)]) {
		if (itemNum_[static_cast<int>(type_)] > 0) {

			use_ = true;
			itemNum_[static_cast<int>(type_)]--;
			
			if (itemNum_[static_cast<int>(type_)] <= 0) {

				int itemNo = static_cast<int>(type_);
				int prevNo = itemNo;
				itemNo++;

				if (itemNo >= static_cast<int>(TYPE::MAX)) {

					itemNo = 0;
				}
				while (itemNum_[itemNo] <= 0) {

					itemNo++;
					if (itemNo >= static_cast<int>(TYPE::MAX)) {

						itemNo = 0;
					}
					if (itemNo == prevNo) {

						itemNo = -1;
						break;
					}
				}
				type_ = static_cast<TYPE>(itemNo);
			}
		}
	}
}

void Item::Draw(void)
{
	float dxF = Application::SCREEN_SIZE_X - 100.0f;
	float dyF = Application::SCREEN_SIZE_Y - 100.0f;
	int dx = static_cast<int>(dxF);
	int dy = static_cast<int>(dyF);
	int itemNo = static_cast<int>(type_);

	DrawBox(dx - 40, dy - 40, dx + 40, dy + 40, 0x222255, true);
	if (itemNo != -1) {
	
		DrawRotaGraph(dxF, dyF, 1.0f, 0.0f, itemImg_[itemNo], true);
	}
	DrawLine(dx - 40, dy - 38, dx + 40, dy - 38, 0xeeee33, 4);
	DrawLine(dx - 38, dy - 36, dx - 38, dy + 40, 0xeeee33, 4);
	DrawLine(dx + 38, dy - 36, dx + 38, dy + 40, 0xeeee33, 4);
	DrawLine(dx - 36, dy + 38, dx + 36, dy + 38, 0xeeee33, 4);
	DrawCircleAA(dxF + 40.0f, dyF + 40.0f, 15.0f, 32, 0x222255);
	SetFontSize(20);
	if (itemNo != -1) {

		int width = GetDrawFormatStringWidth("%d", itemNum_[itemNo]);
		DrawFormatString(dx + 40 - width / 2, dy + 30, 0xffffff, "%d", itemNum_[itemNo]);
	}
	else {

		int width = GetDrawFormatStringWidth("0");
		DrawFormatString(dx + 40 - width / 2, dy + 30, 0xffffff, "0");
	}
}

void Item::Release(void)
{
	for (int i = static_cast<int>(TYPE::MAX) -1; i >= 0; i--){

		DeleteGraph(itemImg_[i]);
	}
}
