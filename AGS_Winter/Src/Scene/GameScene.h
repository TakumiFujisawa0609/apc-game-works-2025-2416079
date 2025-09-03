#pragma once
#include "SceneBase.h"
#include <DxLib.h>


class SceneManager;
class Stage;
class EnemyManager;
class Player;

class GameScene : public SceneBase
{
public:

	static constexpr VECTOR GAMEOVER_POINT = { 450.0f, 30.0f, 75.0f };
	static constexpr float GAMEOVER_RADIUS = 35.0f;

	static constexpr int SHAKE_MAX = 20;

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// ステージ
	Stage* stage_;
	EnemyManager* enemyManager_;
	Player* player_;

	// 画像
	int imgGameover_;
	int imgGameclear_;
	int imgNextwave_;

	bool waveFlg_;
	int waveCnt_;
	int time_;

	bool gameoverFlg_;
	bool gameclearFlg_;

	int effectBlastPlayIdR_;
	int effectBlastPlayIdL_;

	bool shakeFlg_;
	int cntShake_;
	VECTOR prevCameraPos_;

	void Collision(void);
};
