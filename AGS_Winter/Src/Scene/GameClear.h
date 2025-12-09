#pragma once
#include "SceneBase.h"


class SceneManager;

class GameClear : public SceneBase
{

public:

	// コンストラクタ
	GameClear(void);
	// デストラクタ
	~GameClear(void);

	//ロード
	void InitLoad(void) override;
	//その他の初期化
	void Init(void) override;
	//更新
	void Update(void) override;
	//描画
	void Draw(void) override;
	//解放
	void Release(void) override;

private:

	// 画像
	int imgTitle_;
	int imgBg_;
};
