#pragma once
#include <string>
#include <vector>
#include "ColliderBase.h"


class ColliderModel : public ColliderBase
{
public:

	// コンストラクタ
	ColliderModel(const Transform* follow);
	// デストラクタ
	~ColliderModel(void) override;

protected:

	// デバッグ用描画
	void DrawDebug(int color) override {};
};