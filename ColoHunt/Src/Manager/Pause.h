#pragma once
#include "SceneManager.h"

class Pause {
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

	//止まっているかどうか
	bool IsPause() const { return isPause_; }

private:

	enum class PAUSE_STATE {

		CONTINUE = 2,
		SETTING,
		RETURN_TITLE,
		FINISH,
	};

	//メニューの止める位置
	static constexpr int DOWN = 95;

	//ポーズしているか
	bool isPause_;
	//矢印の場所
	PAUSE_STATE state_;

	//現在のメニューの位置
	VECTOR pos_;
};