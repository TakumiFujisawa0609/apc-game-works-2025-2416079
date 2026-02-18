#pragma once
#include "SceneBase.h"


class SceneManager;
class Fader;

class TitleScene : public SceneBase
{

public:

	// コンストラクタ
	TitleScene(void);
	// デストラクタ
	~TitleScene(void);

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

	enum class STATE {

		LOGO,
		SELECT,
		RANKING,
		PV
	};

	enum class SELECT {

		PLAY,
		RANKING,
		END
	};

	Fader* fader_;

	STATE state_;
	SELECT select_;

	std::vector<int> times_;

	static constexpr int CHANGE_CNT = 60;

	// 画像
	int imgTitle_;
	int imgBg_;
	int video_;

	//フォント
	int font_;
	int creatorFont_;
	int selectFont_;
	int timeFont_;

	bool show_;
	int cnt_;

	void ChangeState(STATE sta);

	void UpdateLogo(void);
	void UpdateSelect(void);
	void UpdateRanking(void);
	void UpdatePv(void);

	void ChangeLogo(void);
	void ChangeSelect(void);
	void ChangeRanking(void);
	void ChangePv(void);

	void DrawLogo(void);
	void DrawSelect(void);
	void DrawRanking(void);
	void DrawPv(void);
};
