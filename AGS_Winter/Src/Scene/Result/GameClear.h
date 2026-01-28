#pragma once
#include "ResultBase.h"


class GameClear : public ResultBase
{

public:

	// コンストラクタ
	GameClear(void);
	// デストラクタ
	~GameClear(void);

	//その他の初期化
	void Init(void) override;
	//描画
	void Draw(void) override;
	//解放
	void Release(void) override;

private:

	int timeHandle;
};
