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
	void InitTransform(void) override;
	//コライダの初期化
	void InitCollider(void) override;

	//更新
	void Update(void) override {};

	//透明モデルの描画
	void Draw(void) const override;
	//不透明モデルの描画
	void DrawModel(void) const override;

	//半透明のフレームインデックスの設定
	void SetOpacityIndex(std::vector<int> index);

private:

	//半透明のフレームインデックス
	std::vector<int> opacityIndex;
};