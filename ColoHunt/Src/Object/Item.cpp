#include<DxLib.h>
#include "Item.h"
#include"../Application.h"
#include "../Manager/Input/Controller.h"


Item::Item(void) : itemImg_(),itemNum_(), nowKey_(), prevKey_(), type_(), useNum_(), useType_(), use_()
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
	use_  = false;
	useNum_ = 0;
}

void Item::Update(void)
{
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	// 今のアイテムをintに変換
	int itemNo = static_cast<int>(type_);
	// 今のアイテムの位置を覚える
	int prevNo = itemNo;

	// 右を押したとき
	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT_DPAD)]) {

		// 右にずらす
		itemNo++;

		// 一番右なら
		if (itemNo >= static_cast<int>(TYPE::MAX)) {

			// 一番左に戻す
			itemNo = 0;
		}
		// 移動先のアイテム数がゼロなら
		while (itemNum_[itemNo] <= 0) {

			// 右にずらす
			itemNo++;

			// 一番右なら
			if (itemNo >= static_cast<int>(TYPE::MAX)) {

				// 一番左に戻す
				itemNo = 0;
			}
			// 一周してるなら返す
			if (itemNo == prevNo) {

				break;
			}
		}
	}
	// 左を押したとき
	else if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::LEFT_DPAD)]) {

		// 左にずらす
		itemNo--;

		// 一番左なら
		if (itemNo < 0) {

			// 一番右に戻す
			itemNo = static_cast<int>(TYPE::MAX) - 1;
		}
		// 移動先のアイテム数がゼロなら
		while (itemNum_[itemNo] <= 0) {

			// 左にずらす
			itemNo--;
			
			// 一番左なら
			if (itemNo < 0) {

				// 一番右に戻す
				itemNo = static_cast<int>(TYPE::MAX) - 1;
			}
			// 一周してるなら返す
			if (itemNo == prevNo) {

				break;
			}
		}
	}
	// intをアイテムに戻す
	type_ = static_cast<TYPE>(itemNo);

	// 念のためアイテムがある場合
	if (itemNum_[static_cast<int>(type_)] > 0) {
		// 使うフラグが立ったら
		if (use_) {

			// 使った
			useType_ = type_;
			itemNum_[static_cast<int>(type_)]--;
			useNum_++;
			
			// アイテムがなくなった
			if (itemNum_[static_cast<int>(type_)] <= 0) {

				// 今のアイテムをintに変換
				int itemNo = static_cast<int>(type_);
				// 今のアイテムの位置を覚える
				int prevNo = itemNo;
				// アイテムを右にずらす
				itemNo++;

				// 一番右なら
				if (itemNo >= static_cast<int>(TYPE::MAX)) {

					// 一番左に戻す
					itemNo = 0;
				}
				// 移動先のアイテムがないなら
				while (itemNum_[itemNo] <= 0) {

					// アイテムを右にずらす
					itemNo++;
					
					// 一番右なら
					if (itemNo >= static_cast<int>(TYPE::MAX)) {

						// 一番左に戻す
						itemNo = 0;
					}
					// 一周してるなら
					if (itemNo == prevNo) {

						// 非表示ように-1をいれて返す
						itemNo = -1;
						break;
					}
				}
				type_ = static_cast<TYPE>(itemNo);
			}
		}
	}
	use_ = false;
}

void Item::Draw(void) const
{
	float dxF = Application::SCREEN_SIZE_X - 100.0f;
	float dyF = Application::SCREEN_SIZE_Y - 100.0f;
	int dx = static_cast<int>(dxF);
	int dy = static_cast<int>(dyF);
	int itemNo = static_cast<int>(type_);

	DrawBox(dx - 40, dy - 40, dx + 40, dy + 40, 0x222255, true);
	if (itemNo != -1) {
	
		DrawRotaGraph(dx, dy, 1.0f, 0.0f, itemImg_[itemNo], true);
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

void Item::Release(void) const
{
	for (int i = static_cast<int>(TYPE::MAX) -1; i >= 0; i--){

		DeleteGraph(itemImg_[i]);
	}
}
