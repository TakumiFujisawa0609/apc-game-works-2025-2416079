#pragma once
#include "ActorBase.h"


class Stage : public ActorBase
{
public:

	// コンストラクタ
	Stage(void);
	// デストラクタ
	~Stage(void);

	//ロード
	void InitLoad(void) override;
	//アニメーションの初期化
	void InitAnim(void) override {};
	//その他の初期化
	void InitOwn(void) override;

	//更新
	void Update(void) override {};

	//描画
	void Draw(void) const override {};
};