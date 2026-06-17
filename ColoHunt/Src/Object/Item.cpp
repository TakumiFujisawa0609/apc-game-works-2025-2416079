#include<DxLib.h>
#include "Item.h"
#include"../Application.h"
#include "../Manager/Input/InputManager.h"


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
	// アイテムの初期化
	for (int i = 0; i < static_cast<int>(TYPE::MAX); i++) {

		itemNum_[i] = ITEM_NUM[i];
	}
	type_ = TYPE::HP;
	use_  = false;
	useNum_ = 0;
}

void Item::Update(void)
{
	// 今のアイテムをintに変換
	int itemNo = static_cast<int>(type_);
	// 今のアイテムの位置を覚える
	int prevNo = itemNo;

	// 右を押したとき
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::RIGHT).keyTrgDown) {

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
	else if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::LEFT).keyTrgDown) {

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
	// アイテム描画の相対座標の取得(画像中心)
	float dxF = Application::SCREEN_SIZE_X - 100.0f;
	float dyF = Application::SCREEN_SIZE_Y - 100.0f;
	int dx = static_cast<int>(dxF);
	int dy = static_cast<int>(dyF);
	int itemNo = static_cast<int>(type_);

	// アイテム描画の外枠の描画
	DrawBox(dx + LEFT_POS, dy + UP_POS, dx + RIGHT_POS, dy + DOWN_POS, BOX_COLOR, true);
	DrawLine(dx + LEFT_POS, dy + UP_POS, dx + RIGHT_POS, dy + UP_POS, LINE_COLOR, LINE_WIDTH);
	DrawLine(dx + LEFT_POS, dy + UP_POS, dx + LEFT_POS, dy + DOWN_POS, LINE_COLOR, LINE_WIDTH);
	DrawLine(dx + RIGHT_POS, dy + UP_POS, dx + RIGHT_POS, dy + DOWN_POS, LINE_COLOR, LINE_WIDTH);
	DrawLine(dx + LEFT_POS, dy + DOWN_POS, dx + RIGHT_POS, dy + DOWN_POS, LINE_COLOR, LINE_WIDTH);
	DrawCircleAA(dxF + CIRCLE_POS, dyF + CIRCLE_POS, CIRCLE_RAD, CIRCLE_DIV_NUM, BOX_COLOR);

	// アイテムがあればアイテムの描画
	if (itemNo != -1) {

		DrawRotaGraph(dx, dy, 1.0f, 0.0f, itemImg_[itemNo], true);
	}

	// アイテムの残数の描画
	SetFontSize(20);
	if (itemNo != -1) {

		// 文字幅分だけずらす
		int width = GetDrawFormatStringWidth("%d", itemNum_[itemNo]);
		DrawFormatString(dx + FONT_WIDTH - width / 2, dy + FONT_HEIGHT, FONT_COLOR, "%d", itemNum_[itemNo]);
	}
	else {

		// 文字幅分だけずらす
		int width = GetDrawFormatStringWidth("0");
		DrawFormatString(dx + FONT_WIDTH - width / 2, dy + FONT_HEIGHT, FONT_COLOR, "0");
	}
}

void Item::Release(void) const
{
	for (int i = static_cast<int>(TYPE::MAX) -1; i >= 0; i--){

		DeleteGraph(itemImg_[i]);
	}
}
