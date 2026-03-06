#pragma once
#include "../SceneBase.h"


class SceneManager;

class ResultBase : public SceneBase
{

public:

	// コンストラクタ
	ResultBase(void);
	// デストラクタ
	~ResultBase(void);

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

	//選択状態
	enum class STATE {

		CONTINUE = 2,
		RETURN_TITLE,
		FINISH,
	};

	//背景画像
	int screenImg_;

	//現在の選択状態
	STATE state_;
};
