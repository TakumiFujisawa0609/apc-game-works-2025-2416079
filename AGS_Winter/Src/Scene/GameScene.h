#pragma once
#include "SceneBase.h"
#include <DxLib.h>


class SceneManager;
class Stage;
class EnemyBase;
class Player;

class GameScene : public SceneBase
{
public:
	//カメラとプレイヤーの距離
	static constexpr float CAMERA_TO_PLAYER = 800.0f;

	// コンストラクタ
	GameScene(void);
	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;
	
	void EnemyToPlayer(void);

private:

	// ステージ
	Stage* stage_;
	EnemyBase* enemyBase_;
	Player* player_;

	// 画像
	int imgGameover_;
	int imgGameclear_;
	int imgNextwave_;

	int shadowMap_;

	bool hitFlgP_;
	bool hitFlgE_;

	float yaw_;
	float pitch_;

	bool isFirst_;
	bool isLockon_;

	void Collision(void);
	void GameCamera(void);
	void Effect(MV1_COLL_RESULT_POLY dim);
};
