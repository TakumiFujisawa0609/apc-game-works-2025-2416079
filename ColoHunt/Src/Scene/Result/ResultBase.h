#pragma once
#include "../SceneBase.h"


class SceneManager;

class ResultBase : public SceneBase
{

public:

	enum class STATE {

		CONTINUE = 2,
		RETURN_TITLE,
		FINISH,
	};

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

	int screenImg_;

	STATE state_;
};
