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
	//カメラとプレイヤーの距離
	static constexpr float CAMERA_TO_PLAYER = 800.0f;

	//カメラ感度
	static constexpr float SENSITIVITY = 0.003f;

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

	bool cameraReset_;

	void Collision(void);
	void GameCamera(void);
};
