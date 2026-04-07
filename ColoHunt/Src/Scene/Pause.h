#pragma once
#include <DxLib.h>
#include "SceneBase.h"

class Pause : public SceneBase
{
public:

	//コンストラクタ
	Pause();
	//デストラクタ
	~Pause();

	//ロード
	void InitLoad(void) override {};
	//その他の初期化
	void Init(void) override;
	//更新
	void Update(void) override;
	//描画
	void Draw(void) override;
	//解放
	void Release(void) override;

private:

	enum class STATE {

		NON = 1,

		CONTINUE,
		RETURN_TITLE,
		FINISH,

		MAX,
	};

	//メニューの止める位置
	static constexpr int DOWN = 95;

	//矢印の場所
	STATE state_;

	//現在のメニューの位置
	VECTOR pos_;
};