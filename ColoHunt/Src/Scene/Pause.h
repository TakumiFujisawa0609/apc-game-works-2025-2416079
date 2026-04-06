#pragma once
#include "SceneBase.h"

class Pause : public SceneBase
{
public:

	//コンストラクタ
	Pause();
	//デストラクタ
	~Pause();

	//初期化
	void Init(void);
	//更新
	void Update();
	//描画
	void Draw();
	//開放
	void Release();

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