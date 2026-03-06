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

	//状態
	enum class STATE {

		LOGO,
		SELECT,
		RANKING,
		PV
	};

	//選択
	enum class SELECT {

		PLAY,
		RANKING,
		END
	};

	//フェードのポインター
	Fader* fader_;

	//現在のタイトルの状態
	STATE state_;
	//現在の選択状態
	SELECT select_;

	//保存済みの時間
	std::vector<int> times_;

	//PVが流れるまでの時間
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

	//点滅関係
	bool show_;
	int cnt_;

	//ステータスの変更
	void ChangeState(STATE sta);

	//状態別更新
	void UpdateLogo(void);
	void UpdateSelect(void);
	void UpdateRanking(void);
	void UpdatePv(void);

	//状態別初期化
	void ChangeLogo(void);
	void ChangeSelect(void);
	void ChangeRanking(void);
	void ChangePv(void);

	//状態別描画
	void DrawLogo(void);
	void DrawSelect(void);
	void DrawRanking(void);
	void DrawPv(void);
};
