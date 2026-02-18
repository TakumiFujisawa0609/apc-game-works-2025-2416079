#pragma once
#include "ResultBase.h"


class GameOver : public ResultBase
{

public:

	// コンストラクタ
	GameOver(void);
	// デストラクタ
	~GameOver(void);

	//描画
	void Draw(void) override;
	//解放
	void Release(void) override;

private:

	// 画像
	int img_;
};
