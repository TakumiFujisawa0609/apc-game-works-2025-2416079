#include <DxLib.h>
#include "Item.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"


Item::Item(void)
{
}

Item::~Item(void)
{
}

void Item::Init(void)
{
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
}

void Item::Update(void)
{
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
		}
	}
}

void Item::Draw(void)
{
	float dx = Application::SCREEN_SIZE_X - 100.0f;
	float dy = Application::SCREEN_SIZE_Y - 100.0f;

	DrawFormatString(dx - 40.0f, dy + 40.0f, 0x000000, "%d", itemNum_[static_cast<int>(type_)]);
	DrawBoxAA(dx - 50.0f, dy - 50.0f, dx + 50.0f, dy + 50.0f, 0x222222, true);
	DrawRotaGraph(dx, dy, 1.0f, 0.0f, itemImg_[static_cast<int>(type_)], true);
}

void Item::Release(void)
{
	for (int i = static_cast<int>(TYPE::MAX) -1; i >= 0; i--){

		DeleteGraph(itemImg_[i]);
	}
}
