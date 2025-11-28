#pragma once
#include "SceneBase.h"


class SceneManager;

class GameOver : public SceneBase
{

public:

	// コンストラクタ
	GameOver(void);
	// デストラクタ
	~GameOver(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// 画像
	int img_;
};
