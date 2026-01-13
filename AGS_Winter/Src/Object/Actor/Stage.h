#pragma once
#include "ActorBase.h"
#include <vector>


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
	//モデルの初期化
	void InitModel(void) const override;

	//更新
	void Update(void) override {};

	//描画
	void Draw(void) const override;

	void SetOpacityIndex(std::vector<int> index);

private:

	int opacityModelId_;
	std::vector<int> opacityIndex;
};