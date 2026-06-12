#pragma once
#include <DxLib.h>
#include "SceneBase.h"
#include "../Manager/Audio/AudioManager.h"
#include "../Application.h"


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

	// メニューの左上
	static constexpr int MENU_X = Application::SCREEN_SIZE_X / 3;
	static constexpr int MENU_Y = Application::SCREEN_SIZE_Y / 7;

	// メニューのサイズ
	static constexpr int MENU_SIZE_X = 2;
	static constexpr int MENU_SIZE_Y = 25 / 4;

	// 項目の感覚
	static constexpr int MENU_INTER = Application::SCREEN_SIZE_Y / 5;

	// 矢印の相対座標
	static constexpr int X1_POS = -65;
	static constexpr int X2_POS = -90;
	static constexpr int Y1_POS = 15;
	static constexpr int Y2_POS = 30;

	//矢印の場所
	STATE state_;

	//現在のメニューの位置
	VECTOR pos_;

	// BGMの一時保存
	SoundID pauseBGM_;
};