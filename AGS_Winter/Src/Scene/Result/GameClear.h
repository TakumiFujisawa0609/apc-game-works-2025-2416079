#pragma once
#include "ResultBase.h"


class GameClear : public ResultBase
{

public:

	// コンストラクタ
	GameClear(void);
	// デストラクタ
	~GameClear(void);

	//描画
	void Draw(void) override;
	//解放
	void Release(void) override;

private:
};
